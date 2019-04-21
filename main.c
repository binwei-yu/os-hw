#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include <unistd.h>
#include <pwd.h>


void parse(char* input, char* sep, int* narg, char*** args);
int process(char* cmd);
void error();
char* getLine();
//void findExec()

char buffer[258];
char header[10] = "520sh> ";
const char error_message[30] = "An error has occurred\n";
const char home[10] = "/bin";

int main(int argc, char* argv[]) {
    if (argc != 0 && argc != 1) {
        error();
        exit(1);
    }
    while(1) {
        printf("%s", header);
        char* input = getLine();
//        printf("%s", in);
        if (strcmp(input, "\0") == 0) continue;
        if (!input) {
            printf("Invalid input");
            error();
            continue;
        } else {
            if (strchr(input, '+')) {
                if (strchr(input, ';')) {
                    error();
                    continue;
                } else {
//TODO                  multi-command single threaded
                }
            } else if (strchr(input, ';')){
                if (strchr(input, '+')) {
                    error();
                    continue;
                } else {
//TODO                  multi-command multi threaded
                }
            } else {
//                single command
                process(input);

            }
        }
    }
    return 0;
}

//Process a single user command
int process(char* cmd) {
    char** args;
    int nargs = 0;
    int npath = 0;
    parse(cmd, " ", &nargs, &args);
//    printf("%d", nargs);
//    printf("%s", args[0]);
    // Built in commands
    if (strcmp(*args, "quit") == 0) {
        exit(0);
    } else  if (strcmp(*args, "echo") == 0) {
        if (nargs > 1){
            for (int i = 1; i < nargs; i ++) {
                printf("%s ", args[i]);
            }
        }
        printf("%c", '\n');
    } else if (strcmp(*args, "cd") == 0) {
        char* path = nargs > 1 ? args[1] : getenv(home);
        if (chdir(path) != 0) {
            error();
        }
    } else if (strcmp(*args, "pwd") == 0) {
        char* path = getcwd(NULL, 0);
        printf("%s\n", path);
    } else {
//        printf("Not Built in comamnd");
        pid_t child_pid = fork();
        if (child_pid > 0) {
            if (wait(NULL) != child_pid) {
                error();
            }
        } else if (child_pid == 0) {
            execvp(args[0], args);
            error();
        }
    }
    free(args);
    return 0;
}

//Break input into tokens separated by sep
void parse(char* input, char* sep, int* narg, char*** args) {
    *args = (char**) malloc(128 * sizeof(char*));
    int i = 0;
    char* ele = strtok(input, sep);
    while (ele) {
        (*args)[i++] = ele;
        ele = strtok(NULL, sep);
    }
    (*args)[i] = NULL;
    *narg = i;
}

//Printing error mesages
void error() {
    write(STDERR_FILENO, error_message, strlen(error_message));
}

//Get valid user input smmaler than 256 bytes and remove newline char
char* getLine() {
    if (fgets(buffer, 258, stdin) == NULL) {
        return NULL;
    }
    char* tail = strchr(buffer, '\n');
    if (tail == NULL) {
        return NULL;
    }
    *tail = '\0';
    return buffer;
}
