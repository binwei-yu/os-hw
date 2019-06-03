#ifndef FILESYSTEM_DISK_IMAGE_H
#define FILESYSTEM_DISK_IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <string.h>

#define NINODES 4096
#define NBLOCKS 4096
#define BLOCK_SIZE 4096

typedef struct disk_t {
    int map_inode[NINODES];
    int map_block[NBLOCKS];
    int disk_descriptor;
} disk_t;

typedef struct inode_t {
    int type;
    int size;
    int nblocks;
    int blocks[10];
} inode_t;

typedef struct DirEnt_t {
    int  inum;      // inode number of entry (-1 means entry not used)
    char name[252]; // up to 252 bytes of name in directory (including \0)
} DirEnt_t;

int Disk_Make(char* image_name);
int Disk_Open(char* image_name, disk_t* disk);
int Disk_Read_Block(disk_t *disk, int block, char buf[BLOCK_SIZE]);
int Disk_Write_Block(disk_t *disk, int bnum, char buf[BLOCK_SIZE]);
int Disk_Write_Inode(disk_t* disk, int inum, inode_t* new_node);
int Disk_Read_Inode(disk_t* disk, int inum, inode_t* inode);
int Disk_Free_Block(disk_t *disk, int bnum);
int Disk_Free_Inode(disk_t* disk, int inum);
int Disk_Close(disk_t* disk);


#endif //FILESYSTEM_DISK_IMAGE_H
