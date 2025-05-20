#ifndef MINIEXT2_H
#define MINIEXT2_H

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>

#define BLOCK_SIZE 4096
#define INODE_SIZE 128
#define MAX_NAME 28
#define DIRECT_POINTERS 12
#define TOTAL_BLOCKS 128
#define TOTAL_INODES 16

#pragma pack(push, 1)
struct superblock {
    uint32_t total_blocks;
    uint32_t total_inodes;
    uint32_t block_size;
    uint32_t inode_size;
    uint32_t free_blocks;
    uint32_t free_inodes;
    uint32_t first_data_block;
    char fs_name[16];
};

struct inode {
    uint16_t mode;
    uint16_t uid;
    uint32_t size;
    uint32_t blocks;
    uint32_t direct[12];
    uint32_t indirect;
    uint32_t ctime;
    uint32_t mtime;
    uint16_t links_count;
};

struct dir_entry {
    uint32_t inode;
    char name[MAX_NAME];
};
#pragma pack(pop)

extern int map_base_file(char *filename);
extern int unmap_base_file();

#endif // MINIEXT2_H