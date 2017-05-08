#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

typedef int buffer_type;
int buffer_size;
int number_of_readers;
int number_of_items;

buffer_type * buffer;
pthread_mutex_t * mutex;
pthread_mutex_t mutex_print;

int * items_unread;
int ** reader_item;
int * indexes;

pthread_t * readers;
pthread_t writer;
