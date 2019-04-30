#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <pwd.h>
#include <sys/wait.h>


int parse(char* cmd, char* delimeter, char** instructions);
void process(char* cmd);
void mul_exe(char* cmd);
void prl_exe(char* cmd);
void execute(char* cmd, int mode);
void error(void);

#define MAX_BUFFER_SIZE 256
#define MAX_CMD_NUM 128
#define SGL_MODE 0
#define MUL_MODE 1
#define PRL_MODE 2

const char header[10] = "520sh> ";
const char error_message[30] = "An error has occurred\n";

FILE* file = NULL;

int main(int argc, char* argv[]) {
    // Interactive mode
    if (argc == 1) {
        while(1) {
            /* Get command */
            write(STDOUT_FILENO, header, strlen(header));

            char cmd[MAX_BUFFER_SIZE + 2];
            memset(cmd, 0x00, (MAX_BUFFER_SIZE+2) * sizeof(char));
            fgets(cmd, MAX_BUFFER_SIZE+2, stdin);

            process(cmd);
        }
    }
        // Batch mode
    else if (argc == 2) {
        FILE* bash_file = fopen(argv[1], "r");
        file = bash_file;
        if (bash_file == NULL) {
            error();
            exit(EXIT_FAILURE);
        }
        else {
            char cmd[MAX_BUFFER_SIZE + 2];
            memset(cmd, 0x00, (MAX_BUFFER_SIZE+2) * sizeof(char));
            while(fgets(cmd, MAX_BUFFER_SIZE+2, bash_file)) {
                // Output the command
                if (!strchr(cmd, '\n')) {
                    write(STDOUT_FILENO, cmd, MAX_BUFFER_SIZE + 2);
                    memset(cmd, 0x00, (MAX_BUFFER_SIZE+2) * sizeof(char));
                } else {
                    write(STDOUT_FILENO, cmd, strlen(cmd));
                    
                    process(cmd);
                    
                    memset(cmd, 0x00, (MAX_BUFFER_SIZE+2) * sizeof(char));
                }
            }
            fclose(file);
        }
    }
        // Launch erroneously
    else {
        error();
        exit(EXIT_FAILURE);
    }

    return 0;
}

void process(char* cmd) {
    /* Examine input */
    // Detect overflow
    if (cmd[MAX_BUFFER_SIZE] != '\0' && cmd[MAX_BUFFER_SIZE] != '\n') {
        // Flush stdin buffer
        char c;
        while ((c = getchar()) != EOF && c != '\n');

        error();
        return ;
    }

    // Eliminate newline character
    char* tail = strchr(cmd, '\n');
    if (tail != NULL) *tail = '\0';

    /* Parse command */
    char* multiple_mode = strchr(cmd, ';');
    char* parallel_mode = strchr(cmd, '+');
    // Mixing
    if(multiple_mode && parallel_mode) error();
    else {
        // Multiple command
        if (multiple_mode) mul_exe(cmd);
            // Parallel command
        else if (parallel_mode) prl_exe(cmd);
            // Single command
        else execute(cmd, SGL_MODE);
    }
}

void mul_exe(char* cmd) {
    char* cmds[MAX_CMD_NUM];
    memset(cmds, 0x00, MAX_CMD_NUM * sizeof(char*));

    int cnt = parse(cmd, ";", cmds);
    for(int i = 0; i < cnt; i++)
        execute(cmds[i], MUL_MODE);
}

void prl_exe(char* cmd) {
    char* cmds[MAX_CMD_NUM];
    memset(cmds, 0x00, MAX_CMD_NUM * sizeof(char*));

    int cnt = parse(cmd, "+", cmds);
    for(int i = 0; i < cnt; i++) {
        execute(cmds[i], PRL_MODE);
    }
    for(int i = 0; i < cnt; i++) {
        wait(NULL);
    }
}

void pipe_execute(char* cmd) {
    char* cmds[MAX_CMD_NUM];
    memset(cmds, 0x00, MAX_CMD_NUM * sizeof(char*));
    int argc = parse(cmd, "|", cmds);

    int p[2];
    pid_t pid;
    int fd_in = 0;

    for(int i = 0; i < argc; i++) {
        pipe(p);

        if ((pid = fork()) == -1)
            error();
        else {
            if (pid == 0) {
                // Change the input direction
                dup2(fd_in, 0);
                // Change the output direction
                if (i != argc-1) dup2(p[1], 1);
                close(p[0]);

                char* argv[MAX_CMD_NUM];
                memset(argv, 0x00, MAX_CMD_NUM * sizeof(char*));
                parse(cmds[i], " ", argv);

                execvp(argv[0], argv);

                // Error Handling
                error();
                exit(EXIT_SUCCESS);
            }
            else {
                wait(NULL);
                close(p[1]);
                fd_in = p[0];
            }
        }
    }
}

void execute(char* cmd, int mode) {
    // Support redirection
    int fd_out = dup(STDOUT_FILENO);
    char* tail = strchr(cmd, '>');
    if (tail) {
        char* out[MAX_CMD_NUM];
        memset(out, 0x00, MAX_CMD_NUM * sizeof(char*));
        int argc = parse(tail+1, " ", out);
        *tail = '\0';

        // Legal format
        if (argc == 1) {
            char* path = out[0];

            // Redirect
            int out = open(path, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
            if (out == -1) {
                error();
                return;
            }
            dup2(out, STDOUT_FILENO);
            close(out);
        }
        else {
            error();
            return;
        }
    }

    // Pipeline
    if (strstr(cmd, "||")) {
        error();
        return;
    }
    if (strchr(cmd, '|')) {
        pipe_execute(cmd);
    }
        // Non-pipeline
    else {
        char* argv[MAX_CMD_NUM];
        memset(argv, 0x00, MAX_CMD_NUM * sizeof(char*));
        int argc = parse(cmd, " \t", argv);

        // Skip empty command
        if (argc > 0) {
            char* instr = argv[0];

            // Built-in Commands
            // QUIT
            if (strcmp(instr, "quit") == 0) {
                exit(EXIT_SUCCESS);
            }
                // CD
            else if (strcmp(instr, "cd") == 0) {
                char* path = (argc > 1 ? argv[1] : getenv("HOME"));
                if (chdir(path) != 0) error();
            }
                // PWD
            else if (strcmp(instr, "pwd") == 0) {
                if (argc > 1) {
                    error();
                } else {
                    char* pwd = getcwd(NULL, 0);
                    write(STDOUT_FILENO, pwd, strlen(pwd));
                    write(STDOUT_FILENO, "\n", 1);
                }
            }
                // ECHO
            else if (strcmp(instr, "echo") == 0) {
                write(STDOUT_FILENO, argv[1], strlen(argv[1]));
                write(STDOUT_FILENO, "\n", 1);
            }
            else {
                pid_t pid = fork();
                if (pid == 0) {
                    execvp(argv[0], argv);
                    if(file) fclose(file);
                    error();
                    exit(0);
                }
                else {
                    if (mode == MUL_MODE || mode == SGL_MODE)
                        wait(NULL);
                }
            }
        }
    }
    
    // Restore output destination
    dup2(fd_out, STDOUT_FILENO);
}

// Break input into tokens separated by delimiter
int parse(char* cmd, char* delimeter, char** instructions) {
    // Instruction counter
    int instr_cnt = 0;

    // Seperate the command by delimeter
    char* ptr = strtok(cmd, delimeter);

    int index = 0;
    while(ptr) {
        // Trim leading spaces
        while(isspace((unsigned char)*ptr)) ptr++;

        if (strlen(ptr)) {
            // Trim trailing spaces
            char* end = ptr + strlen(ptr) - 1;
            while(end > ptr && isspace((unsigned char)*end)) {
                *end = '\0';
                end--;
            }

            // Eliminate parenthesis
            while((*ptr == '"' && *end == '"') || (*ptr == '\'' && *end == '\'')) {
                *ptr = *end = '\0';
                ptr++; end--;
            }

            // Save
            instructions[index++] = ptr;

            // Increase counter
            instr_cnt++;
        }
        ptr = strtok(NULL, delimeter);
    }
    return instr_cnt;
}

// Printing error messages
void error() {
    write(STDERR_FILENO, error_message, strlen(error_message));
}
