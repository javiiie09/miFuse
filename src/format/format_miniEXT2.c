#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <miniEXT2.h>

void write_block(int fd, int block_num, void *buf) {
    lseek(fd, block_num * BLOCK_SIZE, SEEK_SET);
    write(fd, buf, BLOCK_SIZE);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <fichero>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // Rellenar el fichero con ceros
    char zero[BLOCK_SIZE] = {0};
    for (int i = 0; i < NUM_BLOCKS; i++) {
        write(fd, zero, BLOCK_SIZE);
    }

    // Inicializar superbloque
    struct superblock sb = {0};
    sb.s_inodes_count = NUM_INODES;
    sb.s_blocks_count = NUM_BLOCKS;
    sb.s_free_blocks_count = NUM_BLOCKS - INODE_TABLE_BLOCK - (NUM_INODES * INODE_SIZE) / BLOCK_SIZE;
    sb.s_free_inodes_count = NUM_INODES - 1; // root ocupado
    sb.s_first_data_block = INODE_TABLE_BLOCK + (NUM_INODES * INODE_SIZE) / BLOCK_SIZE;
    sb.s_log_block_size = 0; // 1024 bytes
    sb.s_inode_size = INODE_SIZE;
    sb.s_magic = 0xEF53;

    write_block(fd, SUPERBLOCK_BLOCK, &sb);

    // Inicializar mapas de bits
    uint8_t inode_bitmap[BLOCK_SIZE] = {0};
    inode_bitmap[0] = 0x01; // inode 0 ocupado (root)
    write_block(fd, INODE_BITMAP_BLOCK, inode_bitmap);

    uint8_t block_bitmap[BLOCK_SIZE] = {0};
    for (int i = 0; i < sb.s_first_data_block; i++)
        block_bitmap[i/8] |= (1 << (i%8)); // bloques de metadatos ocupados
    write_block(fd, BLOCK_BITMAP_BLOCK, block_bitmap);

    // Inicializar tabla de inodos
    struct inode root_inode = {0};
    root_inode.i_mode = 040755; // directorio
    root_inode.i_uid = getuid();
    root_inode.i_gid = getgid();
    root_inode.i_size = BLOCK_SIZE;
    root_inode.i_atime = root_inode.i_ctime = root_inode.i_mtime = time(NULL);
    root_inode.i_links_count = 2;
    root_inode.i_blocks = 1;
    root_inode.i_block[0] = sb.s_first_data_block;

    lseek(fd, INODE_TABLE_BLOCK * BLOCK_SIZE, SEEK_SET);
    write(fd, &root_inode, sizeof(root_inode));

    close(fd);
    printf("Formateo miniEXT2 completado en %s\n", argv[1]);
    return 0;
}