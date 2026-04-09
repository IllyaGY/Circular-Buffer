#pragma once
#include <stddef.h> 


typedef struct circular_buffer{
    void* buffer;
    size_t type_size; 
    int head; 
    int tail; 
    int capacity;
    int count;
} circular_buffer;

int create_cb_array(circular_buffer**, size_t, int); 
int delete_cb_array(circular_buffer**); 

int push_value_array(circular_buffer*, void*);
int pop_value_array(circular_buffer *buf, void*);

int is_empty_array(circular_buffer* buf, int *result); 
int is_full_array(circular_buffer* buf, int *result); 

int get_item_array(circular_buffer*, int index, void **ptr);
int get_size_array(circular_buffer* buf, int *size);
int get_capacity_array(circular_buffer* buf, int *capacity);
