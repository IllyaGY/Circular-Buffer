#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "buffer_array.h"
#include "buffer_log.h"

int create_cb_array(circular_buffer** dest_ptr, size_t type_size, int size){
    if(dest_ptr == NULL){
        CB_LOG_ERROR("The destination pointer is NULL");
        return EFAULT;
    }

    circular_buffer* cb = malloc(sizeof(circular_buffer));
    if(cb == NULL){
        CB_LOG_ERROR("Failed to allocate memory for the buffer struct");
        return ENOMEM;
    }

    cb->buffer = malloc(type_size * size);
    if(cb->buffer == NULL){
        CB_LOG_ERROR("Failed to allocate memory for the buffer");
        free(cb);
        return ENOMEM;
    }

    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    cb->capacity = size;
    cb->type_size = type_size;

    *dest_ptr = cb;

    return 0;
}

int delete_cb_array(circular_buffer** buf){
    if(buf == NULL){
        CB_LOG_ERROR("The pointer to the struct buffer pointer is NULL");
        return EFAULT;
    }
    if(*buf == NULL){
        CB_LOG_ERROR("The struct buffer pointer is NULL");
        return EFAULT;
    }
    if((*buf)->buffer == NULL){
        CB_LOG_ERROR("The buffer pointer is NULL");
        return EFAULT;
    }

    free((*buf)->buffer);
    free(*buf);
    *buf = NULL;

    return 0;
}

int push_value_array(circular_buffer* buf, void* item){
    if(buf == NULL){
        CB_LOG_ERROR("The struct buffer pointer is NULL");
        return EFAULT;
    }
    if(item == NULL){
        CB_LOG_ERROR("The item pointer is NULL");
        return EFAULT;
    }
    if(is_full_array(buf)) {
        CB_LOG_ERROR("The buffer is full");
        return ENOBUFS;
    }

    buf->count++;

    memcpy((char *)buf->buffer + buf->tail * buf->type_size, item, buf->type_size);

    buf->tail++;
    buf->tail %= buf->capacity;

    return 0;
}

int pop_value_array(circular_buffer *buf, void* val){
    if(buf == NULL){
        CB_LOG_ERROR("The struct buffer pointer is NULL");
        return EFAULT;
    }
    if(val == NULL){
        CB_LOG_ERROR("The output value pointer is NULL");
        return EFAULT;
    }
    if(buf->buffer == NULL){
        CB_LOG_ERROR("The buffer pointer is NULL");
        return EFAULT;
    }
    if(is_empty_array(buf))
    {
        CB_LOG_ERROR("The buffer is empty");
        return EAGAIN;
    }

    buf->count--;

    memcpy(val, (char *)buf->buffer + buf->head * buf->type_size, buf->type_size);
    memset((char *)buf->buffer + buf->head * buf->type_size, 0, buf->type_size);

    buf->head++;
    buf->head %= buf->capacity;

    return 0;
}

int is_empty_array(circular_buffer* buf){
    if(buf == NULL) {
        CB_LOG_ERROR("The buffer pointer is null");
        return -1;
    }
    return buf->count == 0;
}

int is_full_array(circular_buffer* buf){
    if(buf == NULL) {
        CB_LOG_ERROR("The buffer pointer is null");
        return -1;
    }
    return buf->count == buf->capacity;
}

int get_item_array(circular_buffer* buf, int index, void** ptr){
    if(buf == NULL)
    {
        CB_LOG_ERROR("The circular buffer struct pointer is NULL");
        return EFAULT;
    }
    if(buf->buffer == NULL) {
        CB_LOG_ERROR("The buffer pointer is NULL");
        return EFAULT;
    }
    if (ptr == NULL)
    {
        CB_LOG_ERROR("The output item pointer is NULL");
        return EFAULT;
    }
    if (index < 0 || index >= buf->count)
    {
        CB_LOG_ERROR("The requested index is out of range");
        return EFAULT;
    }

    int physical_index = (buf->head + index) % buf->capacity;
    *ptr = (char *)buf->buffer + physical_index * buf->type_size;

    return 0;
}

int get_size_array(circular_buffer* buf){
    if (buf == NULL)
    {
        CB_LOG_ERROR("No buffer struct pointer provided");
        return EFAULT;
    }

    return buf->count;
}

int get_capacity_array(circular_buffer* buf){
    if (buf == NULL)
    {
        CB_LOG_ERROR("No buffer struct pointer provided");
        return EFAULT;
    }
    return buf->capacity;
}
