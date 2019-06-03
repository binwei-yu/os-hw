#include "disk_image.h"

// Create a space big enough for the byte maps, inodes and data blocks.
int Disk_Make(char* image_name) {
    int disk_descriptor = open(image_name,  O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    if (disk_descriptor == -1) {
        printf("Disk_Make error: Can not create disk image.\n");
    }
    // byte map for inodes
    char* map_inode[NINODES] = {0};
    if (write(disk_descriptor, map_inode, NINODES * sizeof(int)) == -1) {
        printf("Disk_Make error: can not initialize inode map.\n");
        return -1;
    }
    // byte map for blocks
    char* map_block[NINODES] = {0};
    if (write(disk_descriptor, map_block, NBLOCKS * sizeof(int)) == -1) {
        printf("Disk_Make error: can not initialize block map.\n");
        return -1;
    }
    //  4096 inodes
    inode_t node = (inode_t){0, 0, 0, {0}};
    for (int i = 0; i < NINODES; i ++) {
        if (write(disk_descriptor, &node, sizeof(inode_t)) == -1) {
            printf("Disk_Make error: can not initialize inodes.\n");
            return -1;
        }
    }
    //  4096 data blocks
    char* block[BLOCK_SIZE] = {0};
    for (int i = 0; i < NBLOCKS; i ++) {
        if (write(disk_descriptor, block, BLOCK_SIZE) == -1) {
            printf("Disk_Make error: can not initialize blocks.\n");
            return -1;
        }
    }
    close(disk_descriptor);
    return 0;
}

// Open a disk image and create a disk handler
int Disk_Open(char* image_name, disk_t* disk) {
    int disk_descriptor = open(image_name, O_RDWR);
    if (disk_descriptor == -1) {
        printf("Disk_Open error: can not open disk image.\n");
        return -1;
    }
    if (read(disk_descriptor, disk->map_inode, NINODES * sizeof(int)) == -1) {
        printf("Disk_Open error: can not read inode byte map.\n");
        return -1;
    }
    if (read(disk_descriptor, disk->map_block, NBLOCKS * sizeof(int)) == -1) {
        printf("Disk_Open error: can not read block byte map.\n");
        return -1;
    }
    disk->disk_descriptor = disk_descriptor;
    return 0;
}

// Read a block from the disk to the buffer. Return error if the block is invalid.
int Disk_Read_Block(disk_t *disk, int block, char *buf) {
    if (!disk) return -1;
    // Reading invalid block
    if (block >= NBLOCKS || !disk->map_block[block]) {
        printf("Disk_Read_Block error: invalid data block.\n");
        return -1;
    }
    // Valid block
    int offset = NINODES * sizeof(int) + NBLOCKS * sizeof(int) + NINODES * sizeof(inode_t) + block * BLOCK_SIZE;
    if (lseek(disk->disk_descriptor, offset, SEEK_SET) == -1) {
        printf("Disk_Read_Block error: can not set disk descriptor.\n");
        return -1;
    }
    if (read(disk->disk_descriptor, buf, BLOCK_SIZE) == -1) {
        printf("Disk_Read_Block error: can not read from disk descriptor.\n");
        return -1;
    }
    return 0;
}

int Disk_Read_Inode(disk_t* disk, int inum, inode_t* inode) {
    if (!disk) return -1;
    if (inum >= NINODES || !disk->map_inode[inum]) {
        printf("Disk_Read_Inode error: invalid inode.\n");
        return -1;
    }
    int offset = (NINODES + NBLOCKS) * sizeof(int) + inum * sizeof(inode_t);
    if (lseek(disk->disk_descriptor, offset, SEEK_SET) == -1) {
        printf("Disk_Read_Inode error: can not set disk descriptor.\n");
        return -1;
    }
    if (read(disk->disk_descriptor, inode, sizeof(inode_t)) == -1) {
        printf("Disk_Read_Inode error: can not read from disk descriptor.\n");
        return -1;
    }
    return 0;
}

// Write to the disk from the buffer and update the block byte map.
int Disk_Write_Block(disk_t *disk, int bnum, char buf[BLOCK_SIZE]) {
    if (!disk || !buf) return -1;
    // Invalid data block
    if (bnum >= NBLOCKS) {
        printf("Disk_Write_Block error: invalid data block.\n");
        return -1;
    }
    // Valid block
    int offset = NINODES * sizeof(int) + NBLOCKS * sizeof(int) + NINODES * sizeof(inode_t) + bnum * BLOCK_SIZE;
    if (lseek(disk->disk_descriptor, offset, SEEK_SET) == -1) {
        printf("Disk_Write_Block error: can not set disk descriptor.\n");
        return -1;
    }
    if (write(disk->disk_descriptor, buf, BLOCK_SIZE) == -1) {
        printf("Disk_Write_Block error: can not write to disk descriptor.\n");
        return -1;
    }
    disk->map_block[bnum] = 1;
    if (lseek(disk->disk_descriptor, (NINODES + bnum) * sizeof(int), SEEK_SET) == -1) {
        printf("Disk_Write_Block error: can not set disk descriptor.\n");
        return -1;
    }
    if (write(disk->disk_descriptor, &disk->map_block[bnum], sizeof(int))== -1) {
        printf("Disk_Write_Block error: can not write to disk descriptor.\n");
        return -1;
    }
    return 0;
}

int Disk_Write_Inode(disk_t* disk, int inum, inode_t* new_node) {
    if (!disk || !new_node)
        return -1;
    int offset = (NBLOCKS + NINODES) * sizeof(int) + inum * sizeof(inode_t);
    if (lseek(disk->disk_descriptor, offset, SEEK_SET) == -1) {
        printf("Disk_Write_Inode error: can not set disk discriptor.\n");
        return -1;
    }
    if (write(disk->disk_descriptor, new_node, sizeof(inode_t)) == -1) {
        printf("Disk_Write_Inode error: can not write to disk descriptor.\n");
        return -1;
    }
    disk->map_inode[inum] = 1;
    if (lseek(disk->disk_descriptor, inum * sizeof(int), SEEK_SET) == -1) {
        printf("Disk_Write_Inode error: can not set disk descriptor.\n");
        return -1;
    }
    if (write(disk->disk_descriptor, &disk->map_inode[inum], sizeof(int)) == -1) {
        printf("Disk_Write_Inode error: can not write to disk discriptor.\n");
        return -1;
    }
    return 0;
}

// Free a block from the disk.
int Disk_Free_Block(disk_t *disk, int bnum) {
    if (!disk || bnum < 0 || bnum > NBLOCKS) return -1;
    disk->map_block[bnum] = 0;
    if (lseek(disk->disk_descriptor, (NINODES + bnum) * sizeof(int), SEEK_SET) == -1) {
        printf("Disk_Free_Block error: can not set disk descriptor.\n");
        return -1;
    }
    if (write(disk->disk_descriptor, &disk->map_block[bnum], sizeof(int))== -1) {
        printf("Disk_Write_Block error: can not write to disk descriptor.\n");
        return -1;
    }
    return 0;
}

int Disk_Free_Inode(disk_t* disk, int inum) {
    if (!disk || inum < 0 || inum >= NINODES) return -1;
    disk->map_inode[inum] = 0;
    if (lseek(disk->disk_descriptor, inum * sizeof(int), SEEK_SET) == -1) {
        printf("Disk_Free_Inode error: can not set disk descriptor.\n");
        return -1;
    }
    if (write(disk->disk_descriptor, &disk->map_block[inum], sizeof(int))== -1) {
        printf("Disk_Write_Inode error: can not write to disk descriptor.\n");
        return -1;
    }
    return 0;
}

int Disk_Close(disk_t* disk) {
    close(disk->disk_descriptor);
    return 0;
}

// testing disk performance
//int main() {
//    Disk_Make("./disk_image");
//    disk_t disk;
//    Disk_Open("./disk_image", &disk);
//    for (int i = 0; i < NBLOCKS; i ++) {
//        Disk_Write_Block(&disk, i, "A Big Whale!");
//    }
//    char buf[4096];
//    for (int i = 0; i < NBLOCKS; i ++) {
//        Disk_Read_Block(&disk, i, buf);
//        if (strcmp(buf, "A Big Whale!") != 0 || disk.map_block[i] != 1) {
//            printf("%s:%d\n", "fuck ", i);
//        }
//    }
//    for (int i = 0; i < NBLOCKS; i ++) {
//        Disk_Free_Block(&disk, i);
//    }
//    Disk_Close(&disk);
//    return 0;
//}
