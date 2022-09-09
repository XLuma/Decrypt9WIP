#ifndef DEVCART_H
#define DEVCART_H
#include "../common.h"

//page size mask
#define PAGESIZE_1K 0b00000000
#define PAGESIZE_2K 0b00000001
#define PAGESIZE_4K 0b00000010
#define PAGESIZE_8K 0b00000011

//block size mask
#define BLOCKSIZE_64K 0b00000000
#define BLOCKSIZE_128K 0b00010000
#define BLOCKSIZE_256K 0b00100000
#define BLOCKSIZE_512K 0b00110000

//manufacturer codes
#define TOSHIBA 0x98
#define UNK_1 0xEC

typedef struct s_nand
{
    uint8_t pagesize;
    uint8_t blocksize;
    uint8_t maker_code;
} t_nand;

void get_pagesize(uint8_t num, t_nand *nand);
void get_blocksize(uint8_t num, t_nand *nand);
#endif