#pragma once
#include <stddef.h>


#define ARRAY 1
#define LLIST 2


typedef struct buffer_inf buffer_inf;

typedef void (*cb_iter_fn)(const void*, int);


int create_cb(buffer_inf**, size_t, int, int);
int delete_cb(buffer_inf**); 

int push_value(buffer_inf*, void*);
int pop_value(buffer_inf *buf, void*);

int is_empty(buffer_inf* buf, int *result); 
int is_full(buffer_inf* buf, int *result);

int foreach_value(buffer_inf* buf, cb_iter_fn func); 
int get_size(buffer_inf* buf, int *size);
int get_capacity(buffer_inf* buf, int *capacity);
