#include <errno.h>
#include <stdlib.h>

#include "buffer_api.h"
#include "buffer_array.h"
#include "buffer_list.h"
#include "buffer_log.h"

int create_cb(buffer_inf** buf, size_t type_size, const int size, const int type){
    if (buf == NULL)
    {
        CB_LOG_ERROR("No pointer for the endpoint buffer_inf structure pointer provided");
        return EFAULT;
    }
    if (type != ARRAY && type != LLIST)
    {
        CB_LOG_ERROR("An invalid buffer type was provided");
        return EINVAL;
    }

    *buf = malloc(sizeof(buffer_inf));
    if (*buf == NULL)
    {
        CB_LOG_ERROR("Failed to allocate memory for the buffer_inf struct");
        return ENOMEM;
    }

    (*buf)->type = type;
    (*buf)->buffer = NULL;

    int err = type == ARRAY
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

    int type = (*buf)->type;
    int err = type == ARRAY
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

    return buf->type == ARRAY
        ? pop_value_array(buf->buffer, val)
        : pop_value_list(buf->buffer, val);
}

int is_empty(buffer_inf* buf){
    if(buf == NULL){
        CB_LOG_ERROR("The pointer to the buffer_inf struct is NULL");
        return -1;
    }
    if(buf->buffer == NULL){
        CB_LOG_ERROR("The interface buffer pointer is null");
        return -1;
    }

    return buf->type == ARRAY
        ? is_empty_array(buf->buffer)
        : is_empty_list(buf->buffer);
}

int is_full(buffer_inf* buf){
    if(buf == NULL){
        CB_LOG_ERROR("The pointer to the buffer_inf struct is NULL");
        return -1;
    }
    if(buf->buffer == NULL){
        CB_LOG_ERROR("The interface buffer pointer is NULL");
        return -1;
    }

    return buf->type == ARRAY
        ? is_full_array(buf->buffer)
        : is_full_list(buf->buffer);
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

    int size = buf->type == ARRAY
        ? ((circular_buffer*)(buf->buffer))->count
        : ((circular_list*)(buf->buffer))->count;

    cl_node* cont = NULL;

    for(int i = 0; i < size; i++){
        void *ptr = NULL;
        int err = buf->type == ARRAY
            ? get_item_array(buf->buffer, i, &ptr)
            : get_item_list(buf->buffer, i, &ptr, &cont);

        if (err != 0) {
            return err;
        }

        func(ptr, i);
    }

    return 0;
}

int get_size(buffer_inf* buf){
    if(buf == NULL){
        CB_LOG_ERROR("No buffer_inf struct pointer was provided");
        return EFAULT;
    }
    if (buf->buffer == NULL)
    {
        CB_LOG_ERROR("The interface buffer pointer is null");
        return EFAULT;
    }

    return buf->type == ARRAY
        ? get_size_array(buf->buffer)
        : get_size_list(buf->buffer);
}

int get_capacity(buffer_inf* buf){
    if(buf == NULL){
        CB_LOG_ERROR("No buffer_inf struct pointer was provided");
        return EFAULT;
    }
    if (buf->buffer == NULL)
    {
        CB_LOG_ERROR("The interface buffer pointer is null");
        return EFAULT;
    }

    return buf->type == ARRAY
        ? get_capacity_array(buf->buffer)
        : get_capacity_list(buf->buffer);
}
