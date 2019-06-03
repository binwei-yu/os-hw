#include "filesystem.h"

int find_available_inode(disk_t* disk);
int find_available_block(disk_t* disk);
int create_directory(int pinum, int inum, int bnum);

disk_t disk;

// Initialize the disk image and create a root dir with proper . and .. entries. Root inode = 0
int fs_Init(char* disk_image_name) {
    int exist = access(disk_image_name, 0);
    if (exist == -1) { // If image does not exist, create a new one.
        Disk_Make(disk_image_name);
    }
    if (Disk_Open(disk_image_name, &disk) == -1) {
        printf("fs_Init error: can not open image.\n");
        return -1;
    } // Read the disk image.
    if (disk.map_inode[0] == 0) {
        // create a root directory
        create_directory(0, 0, 0);
    }
    return 0;
}

int fs_Lookup(int pinum, char* name) {
    inode_t parent;
    int size = 0;
    if (Disk_Read_Inode(&disk, pinum, &parent) == -1 || parent.type == TYPE_FILE) {
        printf("invalid pinum\n");
        return -1;
    }
    for (int i = 0; i < parent.nblocks; i ++) {
        int bnum = parent.blocks[i];
        char buf[BLOCK_SIZE];
        Disk_Read_Block(&disk, bnum, buf);
        DirEnt_t temp;
        for (int j = 0; j < BLOCK_SIZE; j += sizeof(DirEnt_t)) {
            memcpy(&temp, buf + j, sizeof(DirEnt_t));
            if (strcmp(name, temp.name) == 0) {
                return temp.inum;
            }
            size += sizeof(DirEnt_t);
            if (size == parent.size)
                return -1;
        }
    }
    return -1;
}

int fs_Create(int pinum, int type, char* name) {
    if (fs_Lookup(pinum, name) != -1)
        return 0;
    char buf[BLOCK_SIZE] = {0};
    if (disk.map_inode[pinum] == 0) {
        printf("fs_Create error: invalid pinum\n");
        return -1;
    }
    inode_t pinode;
    Disk_Read_Inode(&disk, pinum, &pinode);
    if (pinode.type == TYPE_FILE || pinode.size == 10 * BLOCK_SIZE) {
        printf("fs_Create error: can not create with this pinum\n");
        return -1;
    }
    int offset = pinode.size % BLOCK_SIZE;
    int pbnum = 0;
    if (offset == 0) {
        // open a new block
        pbnum = find_available_block(&disk);
        pinode.nblocks ++;
        pinode.blocks[pinode.nblocks - 1] = pbnum;

    } else {
        // read the last block
        pbnum = pinode.blocks[pinode.nblocks - 1];
        Disk_Read_Block(&disk, pbnum, buf);
    }
    pinode.size += sizeof(DirEnt_t);
    int inum = find_available_inode(&disk);
    int bnum = find_available_block(&disk);
    if (inum == -1 || pinum == -1 || (type == TYPE_DIRECTORY && bnum == -1)) {
        printf("fs_Create error: can not find empty inode or block.\n");
        return -1;
    }
    // new directory entry
    DirEnt_t entry = {inum, ""};
    memcpy(entry.name, name, strlen(name) + 1);
    memcpy(buf + offset, &entry, sizeof(DirEnt_t));
    Disk_Write_Inode(&disk, pinum, &pinode);
    Disk_Write_Block(&disk, pbnum, buf);
    // Create a directory
    if (type == TYPE_DIRECTORY) {
        create_directory(pinum, inum, bnum);
    }
    // Create a file
    else {
        inode_t inode = (inode_t) {TYPE_FILE, 0, 0, {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};
        Disk_Write_Inode(&disk, inum, &inode);
    }
    return 0;
}

int create_directory(int pinum, int inum, int bnum) {
    inode_t inode = (inode_t) {TYPE_DIRECTORY, 2 * sizeof(DirEnt_t), 1, {bnum, -1, -1, -1, -1, -1, -1, -1, -1, -1}};
    DirEnt_t parent = {pinum, ".."};
    DirEnt_t current = {inum, "."};
    char buf[BLOCK_SIZE];
    memcpy(buf, &parent, sizeof(DirEnt_t));
    memcpy(buf + sizeof(DirEnt_t), &current, sizeof(DirEnt_t));
    Disk_Write_Block(&disk, bnum, buf);
    Disk_Write_Inode(&disk, inum, &inode);
    return 0;
}


int fs_Unlink(int pinum, char* name) {
    int inum = fs_Lookup(pinum, name);
    // return 0 is inode does not exist in pinum
    if (inum == -1) return 0;
    inode_t pinode;
    inode_t inode;
    Disk_Read_Inode(&disk, pinum, &pinode);
    Disk_Read_Inode(&disk, inum, &inode);
    if (inode.type == TYPE_DIRECTORY) {
        // unlink a directory
        if (inode.size > 2 * sizeof(DirEnt_t)) {
            //return -1 if directory is not -1
            printf("fs_Unlink error: can not unlink non-empty directory.\n");
            return -1;
        }
        Disk_Free_Block(&disk, inode.blocks[0]);
        Disk_Free_Inode(&disk, inum);
    } else {
        // unlink a file
        for (int i = 0; i < 10; i ++) {
            Disk_Free_Block(&disk, inode.blocks[i]);
        }
        Disk_Free_Inode(&disk, inum);
    }

    // remove DirEnt_t from pinode

    char old_buf[BLOCK_SIZE];
    char tail_buf[BLOCK_SIZE];
    int block = 0;
    int offset = 0;
    for (int i = 0; i < pinode.nblocks; i ++) {
        Disk_Read_Block(&disk, pinode.blocks[i], old_buf);
        for (int j = 0; j < BLOCK_SIZE; j += sizeof(DirEnt_t)) {
            DirEnt_t temp;
            memcpy(&temp, old_buf + j, sizeof(DirEnt_t));
            if (strcmp(name, temp.name) == 0) {
                block = pinode.blocks[i];
                offset = j;
            }
        }
    }
    int tail_block = pinode.blocks[pinode.nblocks -1];
    int tail_offset = (pinode.size - sizeof(DirEnt_t)) % BLOCK_SIZE;
    Disk_Read_Block(&disk, tail_block, tail_buf);
    Disk_Read_Block(&disk, block, old_buf);
    memcpy(old_buf + offset, tail_buf + tail_offset, sizeof(DirEnt_t));
    Disk_Write_Block(&disk, block, old_buf);
    pinode.size -= sizeof(DirEnt_t);
    if (pinode.size % BLOCK_SIZE == 0) {
        Disk_Free_Block(&disk, pinode.blocks[pinode.nblocks - 1]);
        pinode.blocks[pinode.nblocks - 1] = -1;
        pinode.nblocks --;
    }
    Disk_Write_Inode(&disk, pinum, &pinode);
    return 0;
}

int fs_Write(int inum, char* buffer, int block) {
    inode_t inode;
    // invalid inode number or directory
    if (Disk_Read_Inode(&disk, inum, &inode) == -1 || inode.type == TYPE_DIRECTORY) {
        printf("fs_Write error: invalid inode\n");
        return -1;
    }
    if (block < 0 || block >= 10) {
        printf("fs_Write error: invalid block.\n");
        return -1;
    }
    int bnum = inode.blocks[block];
    if (bnum == -1) {
        bnum = find_available_block(&disk);
        if (bnum == -1) {
            printf("fs_Write error: can not find empty block.\n");
            return -1;
        }
        inode.nblocks ++;
        inode.blocks[block] = bnum;
    }
    inode.size += BLOCK_SIZE;
    Disk_Write_Inode(&disk, inum, &inode);
    Disk_Write_Block(&disk, bnum, buffer);
    return 0;
}

int fs_Read(int inum, char* buffer, int block) {
    inode_t inode;
    if (Disk_Read_Inode(&disk, inum, &inode) == -1) {
        printf("fs_Read error: invalid inode number.\n");
        return -1;
    }
    if (block < 0 || block >= 10 || inode.blocks[block] == -1) {
        printf("fs_Read error: invalid block number,\n");
        return -1;
    }
    int bnum = inode.blocks[block];
    Disk_Read_Block(&disk, bnum, buffer);
}

int fs_Stat(int inum, char* buffer) {
    inode_t inode;
    if (Disk_Read_Inode(&disk, inum, &inode) == -1) {
        printf("fs_Stat error: invalid inode number.\n");
        return -1;
    }
    memcpy(buffer, &inode, sizeof(inode));
    return 0;

}

int find_available_inode(disk_t* d) {
    for (int i = 0; i < NINODES; i ++) {
        if (d->map_inode[i] == 0) {
            return i;
        }
    }
    return -1;
}

int find_available_block(disk_t* d) {
    for (int i = 0; i < NBLOCKS; i ++) {
        if (d->map_block[i] == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    Disk_Make("./disk_image");
    fs_Init("./disk_image");
    fs_Create(0, TYPE_FILE, "file");
    fs_Write(1, "This is for test.\n", 0);
    fs_Create(0, TYPE_DIRECTORY, "dir");
    fs_Create(2, TYPE_FILE, "file_in_dir");
    char buf[BLOCK_SIZE];
    fs_Read(1, buf, 0);
    printf(buf);
    printf("%d\n", fs_Lookup(0, "file"));
    printf("%d\n", fs_Lookup(0, "dir"));
    printf("%d\n", fs_Lookup(2, "file_in_dir"));
    fs_Unlink(0, "file");
//    fs_Unlink(0, "dir");
    fs_Unlink(2, "file_in_dir");
    fs_Unlink(0, "dir");
    printf("%d\n", fs_Lookup(0, "file"));
    printf("%d\n", fs_Lookup(2, "file_in_dir"));

    return 0;
}