/*
 * fifo.h
 *
 *  Created on: 2014-09-25
 *      Author: QiuWeibo
 */

#ifndef FIFO_H_
#define FIFO_H_

#ifdef __cplusplus
 extern "C" {
#endif 


#ifdef FIFO_GLOBALS
#define FIFO_EXT
#else
#define FIFO_EXT    extern
#endif

#include "stm32f10x.h"

typedef struct
{
    u8 *write_ptr;      // 写指针
    u8 *read_ptr;       // 读指针
    u8 *buffer;         // 指向FIFO首元素地址
    u32 data_size;      // FIFO元素的大小
    u32 fifo_size;      // FIFO大小
    char fifo_empty;    // FIFO空标志
    char fifo_full;     // FIFO满标志
} GP_FIFO_StructDef;

typedef struct
{
    int *write_ptr;      // 写指针
    int *read_ptr;       // 读指针
    int *buffer;         // 指向FIFO首元素地址
    u32 fifo_size;      // FIFO大小(ints)
    char fifo_empty;    // FIFO空标志
    char fifo_full;     // FIFO满标志
} INT_FIFO_StructDef;

typedef struct
{
    char *write_ptr;      // 写指针
    char *read_ptr;       // 读指针
    char *buffer;         // 指向FIFO首元素地址
    u32 fifo_size;      // FIFO大小(bytes)
    char fifo_empty;    // FIFO空标志
    char fifo_full;     // FIFO满标志
} CHAR_FIFO_StructDef;

//genery purpuse fifo
FIFO_EXT void gp_fifo_init( GP_FIFO_StructDef *fifo_ptr,
                            void *buffer_ptr,
                            u32 data_size,
                            u32 fifo_size );
FIFO_EXT int gp_fifo_push(GP_FIFO_StructDef *fifo_ptr, void *buffer_ptr);
FIFO_EXT int gp_fifo_pop(GP_FIFO_StructDef* fifo_ptr, void* buffer_ptr);
FIFO_EXT u32 gp_fifo_count(GP_FIFO_StructDef *fifo_ptr);
FIFO_EXT void gp_fifo_reset(GP_FIFO_StructDef *fifo_ptr);


FIFO_EXT void int_fifo_init( INT_FIFO_StructDef *fifo_ptr, int *buffer_ptr, u32 fifo_size );
FIFO_EXT int int_fifo_push(INT_FIFO_StructDef *fifo_ptr, int *buffer_ptr);
FIFO_EXT int int_fifo_pop(INT_FIFO_StructDef* fifo_ptr, int* buffer_ptr);
FIFO_EXT u32 int_fifo_count(INT_FIFO_StructDef *fifo_ptr);
FIFO_EXT void int_fifo_reset(INT_FIFO_StructDef *fifo_ptr);


FIFO_EXT void char_fifo_init( CHAR_FIFO_StructDef *fifo_ptr, char *buffer_ptr, u32 fifo_size );
FIFO_EXT int char_fifo_push(CHAR_FIFO_StructDef *fifo_ptr, char *buffer_ptr);
FIFO_EXT int char_fifo_pop(CHAR_FIFO_StructDef* fifo_ptr, char* buffer_ptr);
FIFO_EXT u32 char_fifo_count(CHAR_FIFO_StructDef *fifo_ptr);
FIFO_EXT void char_fifo_reset(CHAR_FIFO_StructDef *fifo_ptr);

#ifdef __cplusplus
}
#endif

#endif /* FIFO_H_ */

