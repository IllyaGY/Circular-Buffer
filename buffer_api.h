#pragma once
#include <stddef.h>


#define ARRAY 1
#define LLIST 2

typedef struct buffer_inf{
    void* buffer;
    int type;
} buffer_inf; 

typedef void (*cb_iter_fn)(const void* item, int i);


int create_cb(buffer_inf**, size_t, int, int);
int delete_cb(buffer_inf**); 

int push_value(buffer_inf*, void*);
int pop_value(buffer_inf *buf, void*);

int is_empty(buffer_inf* buf); 
int is_full(buffer_inf* buf);

int foreach_value(buffer_inf* buf, cb_iter_fn func); 
int get_size(buffer_inf* buf);
int get_capacity(buffer_inf* buf);
