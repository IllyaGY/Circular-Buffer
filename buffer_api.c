#include <errno.h>
#include <stdlib.h>

#include "buffer_api.h"
#include "buffer_array.h"
#include "buffer_list.h"
#include "buffer_log.h"

struct buffer_inf {
    void *buffer;
    int type;
};

static int validate_buffer_type(int type){
    if (type != ARRAY && type != LLIST)
    {
        CB_LOG_ERROR("An invalid buffer type was provided");
        return EINVAL;
    }

    return 0;
}

int create_cb(buffer_inf** buf, size_t type_size, const int size, const int type){
    if (buf == NULL)
    {
        CB_LOG_ERROR("No pointer for the endpoint buffer_inf structure pointer provided");
        return EFAULT;
    }
    if (type_size == 0)
    {
        CB_LOG_ERROR("The element size must be greater than zero");
        return EINVAL;
    }
    if (size <= 0)
    {
        CB_LOG_ERROR("The buffer capacity must be greater than zero");
        return EINVAL;
    }

    int err = validate_buffer_type(type);
    if (err != 0)
    {
        return err;
    }

    *buf = malloc(sizeof(buffer_inf));
    if (*buf == NULL)
    {
        CB_LOG_ERROR("Failed to allocate memory for the buffer_inf struct");
        return ENOMEM;
    }

    (*buf)->type = type;
    (*buf)->buffer = NULL;

    err = type == ARRAY
        ? create_cb_array((circular_buffer**)&(*buf)->buffer, type_size, size)
        : create_cb_list((circular_list**)&(*buf)->buffer, type_size, size);

    if (err != 0) {
        free(*buf);
        *buf = NULL;
    }

    return err;
}

int delete_cb(buffer_inf** buf){
    if(buf == NULL){
        CB_LOG_ERROR("The pointer to the buffer_inf struct pointer is NULL");
        return EFAULT;
    }
    if(*buf == NULL){
        CB_LOG_ERROR("The pointer to the buffer_inf struct is NULL");
        return EFAULT;
    }
    if((*buf)->buffer == NULL){
        CB_LOG_ERROR("The interface buffer pointer is null");
        return EFAULT;
    }

    int err = validate_buffer_type((*buf)->type);
    if (err != 0)
    {
        return err;
    }

    int type = (*buf)->type;
    err = type == ARRAY
        ? delete_cb_array((circular_buffer**)&(*buf)->buffer)
        : delete_cb_list((circular_list**)&(*buf)->buffer);

    free(*buf);
    *buf = NULL;

    return err;
}

int push_value(buffer_inf* buf, void* item){
    if(buf == NULL){
        CB_LOG_ERROR("The pointer to the buffer_inf struct is NULL");
        return EFAULT;
    }
    if(buf->buffer == NULL){
        CB_LOG_ERROR("The interface buffer pointer is null");
        return EFAULT;
    }

    int err = validate_buffer_type(buf->type);
    if (err != 0)
    {
        return err;
    }

    return buf->type == ARRAY
        ? push_value_array(buf->buffer, item)
        : push_value_list(buf->buffer, item);
}

int pop_value(buffer_inf *buf, void* val){
    if(buf == NULL){
        CB_LOG_ERROR("The pointer to the buffer_inf struct is NULL");
        return EFAULT;
    }
    if(val == NULL){
        CB_LOG_ERROR("The output value pointer is NULL");
        return EFAULT;
    }
    if(buf->buffer == NULL){
        CB_LOG_ERROR("The interface buffer pointer is null");
        return EFAULT;
    }

    int err = validate_buffer_type(buf->type);
    if (err != 0)
    {
        return err;
    }

    return buf->type == ARRAY
        ? pop_value_array(buf->buffer, val)
        : pop_value_list(buf->buffer, val);
}

int is_empty(buffer_inf* buf, int *result){
    if(buf == NULL){
        CB_LOG_ERROR("The pointer to the buffer_inf struct is NULL");
        return EFAULT;
    }
    if(buf->buffer == NULL){
        CB_LOG_ERROR("The interface buffer pointer is null");
        return EFAULT;
    }
    if(result == NULL){
        CB_LOG_ERROR("The output result pointer is NULL");
        return EFAULT;
    }

    int err = validate_buffer_type(buf->type);
    if (err != 0)
    {
        return err;
    }

    return buf->type == ARRAY
        ? is_empty_array(buf->buffer, result)
        : is_empty_list(buf->buffer, result);
}

int is_full(buffer_inf* buf, int *result){
    if(buf == NULL){
        CB_LOG_ERROR("The pointer to the buffer_inf struct is NULL");
        return EFAULT;
    }
    if(buf->buffer == NULL){
        CB_LOG_ERROR("The interface buffer pointer is NULL");
        return EFAULT;
    }
    if(result == NULL){
        CB_LOG_ERROR("The output result pointer is NULL");
        return EFAULT;
    }

    int err = validate_buffer_type(buf->type);
    if (err != 0)
    {
        return err;
    }

    return buf->type == ARRAY
        ? is_full_array(buf->buffer, result)
        : is_full_list(buf->buffer, result);
}

int foreach_value(buffer_inf* buf, cb_iter_fn func){
    if (buf == NULL)
    {
        CB_LOG_ERROR("No buffer_inf struct pointer was provided");
        return EFAULT;
    }
    if (buf->buffer == NULL)
    {
        CB_LOG_ERROR("The interface buffer pointer is null");
        return EFAULT;
    }
    if (func == NULL)
    {
        CB_LOG_ERROR("No callback function was provided");
        return EFAULT;
    }

    int err = validate_buffer_type(buf->type);
    if (err != 0)
    {
        return err;
    }

    int size = 0;
    err = get_size(buf, &size);
    if (err != 0)
    {
        return err;
    }

    cl_node* cont = NULL;

    for(int i = 0; i < size; i++){
        void *ptr = NULL;
        err = buf->type == ARRAY
            ? get_item_array(buf->buffer, i, &ptr)
            : get_item_list(buf->buffer, i, &ptr, &cont);

        if (err != 0) {
            return err;
        }

        func(ptr, i);
    }

    return 0;
}

int get_size(buffer_inf* buf, int *size){
    if(buf == NULL){
        CB_LOG_ERROR("No buffer_inf struct pointer was provided");
        return EFAULT;
    }
    if (buf->buffer == NULL)
    {
        CB_LOG_ERROR("The interface buffer pointer is null");
        return EFAULT;
    }
    if (size == NULL)
    {
        CB_LOG_ERROR("The output size pointer is NULL");
        return EFAULT;
    }

    int err = validate_buffer_type(buf->type);
    if (err != 0)
    {
        return err;
    }

    return buf->type == ARRAY
        ? get_size_array(buf->buffer, size)
        : get_size_list(buf->buffer, size);
}

int get_capacity(buffer_inf* buf, int *capacity){
    if(buf == NULL){
        CB_LOG_ERROR("No buffer_inf struct pointer was provided");
        return EFAULT;
    }
    if (buf->buffer == NULL)
    {
        CB_LOG_ERROR("The interface buffer pointer is null");
        return EFAULT;
    }
    if (capacity == NULL)
    {
        CB_LOG_ERROR("The output capacity pointer is NULL");
        return EFAULT;
    }

    int err = validate_buffer_type(buf->type);
    if (err != 0)
    {
        return err;
    }

    return buf->type == ARRAY
        ? get_capacity_array(buf->buffer, capacity)
        : get_capacity_list(buf->buffer, capacity);
}
