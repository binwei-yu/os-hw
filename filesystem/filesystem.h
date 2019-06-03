#ifndef FILESYSTEM_FILESYSTEM_H
#define FILESYSTEM_FILESYSTEM_H

#include "disk_image.h"

#define TYPE_DIRECTORY 0
#define TYPE_FILE 1

int fs_Init(char* disk_image_name);

int fs_Lookup(int pinum, char* name);

int fs_Stat(int inum, char* buffer);

int fs_Create(int pinum, int type, char* name);

int fs_Unlink(int pinum, char* name);

int fs_Write(int inum, char* buffer, int block);

int fs_Read(int inum, char* buffer, int block);

#endif //FILESYSTEM_FILESYSTEM_H
