/*
 * fifo.c
 *
 *  Created on: 2013-09-25
 *      Author: QiuWeibo
 */
#define FIFO_GLOBALS
#include "fifo.h"
#include "includes.h"

//#define FIFO_DEBUG_INFO
#ifdef FIFO_DEBUG_INFO
#define FIFO_DBG_INFO(fmt, arg...) udprintf("\r\n[FIFO] "fmt, ##arg)
#else
#define FIFO_DBG_INFO(fmt, arg...)
#endif

void gp_fifo_init(  GP_FIFO_StructDef *fifo_ptr,
                    void *buffer_ptr,
                    u32 data_size,
                    u32 fifo_size )
{
    if (NULL == fifo_ptr || NULL == buffer_ptr) return;
    fifo_ptr->buffer        = buffer_ptr;
    fifo_ptr->write_ptr     = fifo_ptr->buffer;
    fifo_ptr->read_ptr      = fifo_ptr->buffer;
    fifo_ptr->data_size     = data_size;
    fifo_ptr->fifo_size     = fifo_size;
    fifo_ptr->fifo_empty    = 1;
    fifo_ptr->fifo_full     = 0;
    memset(fifo_ptr->buffer, 0, (data_size * fifo_size));
    FIFO_DBG_INFO("gp_fifo_init data_size=%d, fifo_size=%d",data_size, fifo_size);
}

int gp_fifo_push(GP_FIFO_StructDef *fifo_ptr, void *buffer_ptr)
{
    if (NULL == fifo_ptr || NULL == buffer_ptr) return -1;
    if(!fifo_ptr->fifo_full)
    {
        memcpy(fifo_ptr->write_ptr, buffer_ptr, fifo_ptr->data_size);
        
        fifo_ptr->write_ptr += fifo_ptr->data_size;
        if(fifo_ptr->write_ptr >= fifo_ptr->buffer + fifo_ptr->data_size * fifo_ptr->fifo_size)
            fifo_ptr->write_ptr = fifo_ptr->buffer;
        if(fifo_ptr->write_ptr == fifo_ptr->read_ptr)
        {
            fifo_ptr->fifo_full=1;
        }
        fifo_ptr->fifo_empty=0;
        return 0;
    }
    else //fifo is full
    {
        FIFO_DBG_INFO("gp_fifo_push Full Error");
        return -1;
    }
}

//@return : 0 -- OK,  -1 -- Error
int gp_fifo_pop(GP_FIFO_StructDef* fifo_ptr, void* buffer_ptr)
{
    if (NULL == fifo_ptr || NULL == buffer_ptr) return -1;
    
    if(!fifo_ptr->fifo_empty)
    {
        memcpy(buffer_ptr, fifo_ptr->read_ptr, fifo_ptr->data_size);
        
        fifo_ptr->read_ptr += fifo_ptr->data_size;
        if(fifo_ptr->read_ptr >= fifo_ptr->buffer + fifo_ptr->data_size * fifo_ptr->fifo_size)
            fifo_ptr->read_ptr = fifo_ptr->buffer;
        if(fifo_ptr->read_ptr == fifo_ptr->write_ptr)
        {
            fifo_ptr->fifo_empty = 1;
        }
        fifo_ptr->fifo_full = 0;
        return 0;
    }
    else
    {
        return -1;
    }
}

u32 gp_fifo_count(GP_FIFO_StructDef *fifo_ptr)
{
    if (NULL == fifo_ptr) return 0;
    
    if(fifo_ptr->fifo_full)
    {
        return fifo_ptr->fifo_size;
    }
    else
    {
        if(fifo_ptr->write_ptr >= fifo_ptr->read_ptr)
        {
            return(fifo_ptr->write_ptr - fifo_ptr->read_ptr)/fifo_ptr->data_size;
        }
        else
        {
            return fifo_ptr->fifo_size-(fifo_ptr->read_ptr - fifo_ptr->write_ptr) / fifo_ptr->data_size;
        }
    }
}

void gp_fifo_reset(GP_FIFO_StructDef *fifo_ptr)
{
    if (NULL == fifo_ptr) return;
    
    fifo_ptr->write_ptr     = fifo_ptr->buffer;
    fifo_ptr->read_ptr      = fifo_ptr->buffer;
    fifo_ptr->fifo_empty    = 1;
    fifo_ptr->fifo_full     = 0;
}


void int_fifo_init( INT_FIFO_StructDef *fifo_ptr, int *buffer_ptr, u32 fifo_size )
{
    if (NULL == fifo_ptr || NULL == buffer_ptr) return;
    fifo_ptr->buffer        = buffer_ptr;
    fifo_ptr->write_ptr     = fifo_ptr->buffer;
    fifo_ptr->read_ptr      = fifo_ptr->buffer;
    fifo_ptr->fifo_size     = fifo_size;
    fifo_ptr->fifo_empty    = 1;
    fifo_ptr->fifo_full     = 0;
    memset(fifo_ptr->buffer, 0, (sizeof(int) * fifo_size));
    FIFO_DBG_INFO("gp_fifo_init data_size=%d, fifo_size=%d",data_size, fifo_size);
}

int int_fifo_push(INT_FIFO_StructDef *fifo_ptr, int *buffer_ptr)
{
    if (NULL == fifo_ptr || NULL == buffer_ptr) return -1;
    
    if(!fifo_ptr->fifo_full)
    {
        *fifo_ptr->write_ptr = *buffer_ptr;
        fifo_ptr->write_ptr++;
        if(fifo_ptr->write_ptr >= fifo_ptr->buffer + sizeof(int) * fifo_ptr->fifo_size)
            fifo_ptr->write_ptr = fifo_ptr->buffer;
        if(fifo_ptr->write_ptr == fifo_ptr->read_ptr)
        {
            fifo_ptr->fifo_full=1;
        }
        fifo_ptr->fifo_empty=0;
        return 0;
    }
    else //fifo is full
    {
        FIFO_DBG_INFO("gp_fifo_push Full Error");
        return -1;
    }
}

int int_fifo_pop(INT_FIFO_StructDef* fifo_ptr, int* buffer_ptr)
{
    if (NULL == fifo_ptr || NULL == buffer_ptr) return -1;
    
    if(!fifo_ptr->fifo_empty)
    {
        *buffer_ptr = *fifo_ptr->read_ptr;
        
        fifo_ptr->read_ptr ++;
        if(fifo_ptr->read_ptr >= fifo_ptr->buffer + sizeof(int) * fifo_ptr->fifo_size)
            fifo_ptr->read_ptr = fifo_ptr->buffer;
        if(fifo_ptr->read_ptr == fifo_ptr->write_ptr)
        {
            fifo_ptr->fifo_empty = 1;
        }
        fifo_ptr->fifo_full = 0;
        return 0;
    }
    else
    {
        return -1;
    }
}

u32 int_fifo_count(INT_FIFO_StructDef *fifo_ptr)
{
    if (NULL == fifo_ptr) return 0;
    
    if(fifo_ptr->fifo_full)
    {
        return fifo_ptr->fifo_size;
    }
    else
    {
        if(fifo_ptr->write_ptr >= fifo_ptr->read_ptr)
        {
            return (fifo_ptr->write_ptr - fifo_ptr->read_ptr)/sizeof(int);
        }
        else
        {
            return fifo_ptr->fifo_size-(fifo_ptr->read_ptr - fifo_ptr->write_ptr) / sizeof(int);
        }
    }
}

void int_fifo_reset(INT_FIFO_StructDef *fifo_ptr)
{
    if (NULL == fifo_ptr) return;
    
    fifo_ptr->write_ptr     = fifo_ptr->buffer;
    fifo_ptr->read_ptr      = fifo_ptr->buffer;
    fifo_ptr->fifo_empty    = 1;
    fifo_ptr->fifo_full     = 0;
}


void char_fifo_init( CHAR_FIFO_StructDef *fifo_ptr, char *buffer_ptr, u32 fifo_size )
{
    if (NULL == fifo_ptr || NULL == buffer_ptr) return;
    fifo_ptr->buffer        = buffer_ptr;
    fifo_ptr->write_ptr     = fifo_ptr->buffer;
    fifo_ptr->read_ptr      = fifo_ptr->buffer;
    fifo_ptr->fifo_size     = fifo_size;
    fifo_ptr->fifo_empty    = 1;
    fifo_ptr->fifo_full     = 0;
    memset(fifo_ptr->buffer, 0, fifo_size);
    FIFO_DBG_INFO("gp_fifo_init data_size=%d, fifo_size=%d",data_size, fifo_size);
}

int char_fifo_push(CHAR_FIFO_StructDef *fifo_ptr, char *buffer_ptr)
{
    if (NULL == fifo_ptr || NULL == buffer_ptr) return -1;
    
    if(!fifo_ptr->fifo_full)
    {
        *fifo_ptr->write_ptr = *buffer_ptr;
        fifo_ptr->write_ptr++;
        if(fifo_ptr->write_ptr >= (fifo_ptr->buffer + fifo_ptr->fifo_size))
            fifo_ptr->write_ptr = fifo_ptr->buffer;
        if(fifo_ptr->write_ptr == fifo_ptr->read_ptr)
        {
            fifo_ptr->fifo_full=1;
        }
        fifo_ptr->fifo_empty=0;
        return 0;
    }
    else //fifo is full
    {
        FIFO_DBG_INFO("gp_fifo_push Full Error");
        return -1;
    }
}

int char_fifo_pop(CHAR_FIFO_StructDef* fifo_ptr, char* buffer_ptr)
{
    if (NULL == fifo_ptr || NULL == buffer_ptr) return -1;
    
    if(!fifo_ptr->fifo_empty)
    {
        *buffer_ptr = *fifo_ptr->read_ptr;
        
        fifo_ptr->read_ptr ++;
        if(fifo_ptr->read_ptr >= (fifo_ptr->buffer + fifo_ptr->fifo_size))
            fifo_ptr->read_ptr = fifo_ptr->buffer;
        if(fifo_ptr->read_ptr == fifo_ptr->write_ptr)
        {
            fifo_ptr->fifo_empty = 1;
        }
        fifo_ptr->fifo_full = 0;
        return 0;
    }
    else
    {
        return -1;
    }
}

u32 char_fifo_count(CHAR_FIFO_StructDef *fifo_ptr)
{
    if (NULL == fifo_ptr) return 0;
    
    if(fifo_ptr->fifo_full)
    {
        return fifo_ptr->fifo_size;
    }
    else
    {
        if(fifo_ptr->write_ptr >= fifo_ptr->read_ptr)
        {
            return (fifo_ptr->write_ptr - fifo_ptr->read_ptr);
        }
        else
        {
            return fifo_ptr->fifo_size-(fifo_ptr->read_ptr - fifo_ptr->write_ptr);
        }
    }
}

void char_fifo_reset(CHAR_FIFO_StructDef *fifo_ptr)
{
    if (NULL == fifo_ptr) return;
    
    fifo_ptr->write_ptr     = fifo_ptr->buffer;
    fifo_ptr->read_ptr      = fifo_ptr->buffer;
    fifo_ptr->fifo_empty    = 1;
    fifo_ptr->fifo_full     = 0;
}
