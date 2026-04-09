#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "buffer_list.h"
#include "buffer_log.h"

static int create_cb_node(cl_node** dest_ptr, size_t type_size, cl_node *next, cl_node* prev){
    if(dest_ptr == NULL){
        CB_LOG_ERROR("The destination pointer is NULL");
        return EFAULT;
    }

    cl_node *node = malloc(sizeof(cl_node));
    if(node == NULL){
        CB_LOG_ERROR("Failed to allocate memory for the node struct");
        return ENOMEM;
    }

    node->item = malloc(type_size);
    if(node->item == NULL){
        CB_LOG_ERROR("Failed to allocate memory for the node's item space");
        free(node);
        return ENOMEM;
    }

    node->next = next;
    node->prev = prev;
    *dest_ptr = node;

    return 0;
}

int create_cb_list(circular_list** dest_ptr, size_t type_size, int size){
    if(dest_ptr == NULL){
        CB_LOG_ERROR("The destination pointer is NULL");
        return EFAULT;
    }
    if(type_size == 0){
        CB_LOG_ERROR("The element size must be greater than zero");
        return EINVAL;
    }
    if(size <= 0){
        CB_LOG_ERROR("The buffer capacity must be greater than zero");
        return EINVAL;
    }

    circular_list* cl = malloc(sizeof(circular_list));
    if(cl == NULL){
        CB_LOG_ERROR("Failed to allocate memory for the list buffer struct");
        return ENOMEM;
    }

    int err = create_cb_node(&cl->head, type_size, NULL, NULL);
    if(err != 0){
        CB_LOG_ERROR("Failed to allocate memory for the list buffer");
        free(cl);
        return ENOMEM;
    }

    cl->tail = cl->head;
    cl->count = 0;
    cl->capacity = size;
    cl->type_size = type_size;

    *dest_ptr = cl;

    return 0;
}

int delete_cb_list(circular_list** buf){
    if(buf == NULL){
        CB_LOG_ERROR("The pointer to the list buffer pointer is NULL");
        return EFAULT;
    }
    if(*buf == NULL){
        CB_LOG_ERROR("The list buffer pointer is NULL");
        return EFAULT;
    }
    if((*buf)->head == NULL || (*buf)->tail == NULL){
        CB_LOG_ERROR("The head or tail of the buffer is NULL");
        return EFAULT;
    }

    cl_node* node = (*buf)->head;
    while(node != NULL){
        cl_node* tmp = node->next;
        free(node->item);
        free(node);
        node = tmp;
    }

    free(*buf);
    *buf = NULL;

    return 0;
}

int push_value_list(circular_list* buf, void* item){
    if(buf == NULL){
        CB_LOG_ERROR("The list buffer pointer is NULL");
        return EFAULT;
    }
    if(buf->tail == NULL){
        CB_LOG_ERROR("The tail node pointer is NULL");
        return EFAULT;
    }
    if(item == NULL){
        CB_LOG_ERROR("The item pointer is NULL");
        return EFAULT;
    }
    int is_full = 0;
    int err = is_full_list(buf, &is_full);
    if(err != 0){
        return err;
    }
    if(is_full) {
        CB_LOG_ERROR("The buffer is full");
        return ENOBUFS;
    }

    if(buf->tail->next == NULL) {
        int er = create_cb_node(&(buf->tail->next), buf->type_size, NULL, buf->tail);
        if (er != 0) {
            CB_LOG_ERROR("Node creation failed");
            return er;
        }
    }

    memcpy((char *)buf->tail->item, item, buf->type_size);

    buf->count++;
    buf->tail = buf->tail->next;

    return 0;
}

int pop_value_list(circular_list *buf, void* val){
    if(buf == NULL){
        CB_LOG_ERROR("The pointer to the struct buffer pointer is NULL");
        return EFAULT;
    }
    if(val == NULL){
        CB_LOG_ERROR("The output value pointer is NULL");
        return EFAULT;
    }
    int is_empty = 0;
    int err = is_empty_list(buf, &is_empty);
    if(err != 0){
        return err;
    }
    if(is_empty)
    {
        CB_LOG_ERROR("The list buffer is empty");
        return EAGAIN;
    }
    if(buf->head == NULL || buf->tail == NULL){
        CB_LOG_ERROR("The head or tail of the buffer is NULL");
        return EFAULT;
    }

    cl_node *node = buf->head;
    memcpy(val, (char *)node->item, buf->type_size);

    if(node->next == NULL){
        int er = create_cb_node(&node->next, buf->type_size, NULL, NULL);
        if(er != 0){
            CB_LOG_ERROR("Error creating a new node during pop");
            return er;
        }
        buf->head = node->next;
        buf->tail = buf->head;
    }
    else {
        buf->head = buf->head->next;
        buf->head->prev = NULL;
    }

    free(node->item);
    free(node);

    buf->count--;

    return 0;
}

int is_empty_list(circular_list* buf, int *result){
    if(buf == NULL) {
        CB_LOG_ERROR("The list buffer struct pointer is null");
        return EFAULT;
    }
    if(result == NULL){
        CB_LOG_ERROR("The output result pointer is NULL");
        return EFAULT;
    }

    *result = buf->count == 0;
    return 0;
}

int is_full_list(circular_list* buf, int *result){
    if(buf == NULL) {
        CB_LOG_ERROR("The list buffer struct pointer is null");
        return EFAULT;
    }
    if(result == NULL){
        CB_LOG_ERROR("The output result pointer is NULL");
        return EFAULT;
    }

    *result = buf->count == buf->capacity;
    return 0;
}

int get_item_list(circular_list* buf, int index, void** ptr, cl_node** cont)
{
    if (buf == NULL)
    {
        CB_LOG_ERROR("The pointer to circular list struct pointer is NULL");
        return EFAULT;
    }
    if (ptr == NULL)
    {
        CB_LOG_ERROR("The output item pointer is NULL");
        return EFAULT;
    }
    if (cont == NULL)
    {
        CB_LOG_ERROR("The continuation pointer is NULL");
        return EFAULT;
    }
    if (index < 0 || index >= buf->count)
    {
        CB_LOG_ERROR("The requested index is out of range");
        return EFAULT;
    }

    cl_node* stepper = buf->head;
    if (*cont != NULL)
    {
        stepper = (*cont);
        *cont = (*cont)->next;
        stepper = stepper->next;
    }
    else
    {
        for(int i = 0; i < index && stepper != NULL; i++)
            stepper = stepper->next;
    }

    if (stepper == NULL || stepper->item == NULL)
    {
        *cont = NULL;
        CB_LOG_ERROR("The requested item could not be resolved");
        return EFAULT;
    }

    if (*cont == NULL)
    {
        *cont = stepper;
    }

    *ptr = stepper->item;

    return 0;
}

int get_size_list(circular_list* buf, int *size){
    if (buf == NULL)
    {
        CB_LOG_ERROR("No buffer struct pointer provided");
        return EFAULT;
    }
    if (size == NULL)
    {
        CB_LOG_ERROR("The output size pointer is NULL");
        return EFAULT;
    }

    *size = buf->count;
    return 0;
}

int get_capacity_list(circular_list* buf, int *capacity){
    if (buf == NULL)
    {
        CB_LOG_ERROR("No buffer struct pointer provided");
        return EFAULT;
    }
    if (capacity == NULL)
    {
        CB_LOG_ERROR("The output capacity pointer is NULL");
        return EFAULT;
    }

    *capacity = buf->capacity;
    return 0;
}
