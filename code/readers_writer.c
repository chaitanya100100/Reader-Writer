#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include "readers_writer.h"

buffer_type get_buffer_item()
{
  return rand()%1000;
}
void display_buffer_item(buffer_type item)
{
  printf("item is : %d\n", item);
}

void initialize()
{
  buffer = (buffer_type*) malloc( sizeof(buffer_type) * buffer_size );
  mutex = (pthread_mutex_t*) malloc( sizeof(pthread_mutex_t) * buffer_size );
  pthread_mutex_init(&mutex_print, NULL);
  items_unread = (int*) malloc( sizeof(int) * buffer_size );
  for(int i=0; i<buffer_size; i++)
  {
    items_unread[i] = 0;
    buffer[i] = -1;
    pthread_mutex_init( &mutex[i], NULL );
  }

  readers = (pthread_t*) malloc( sizeof(pthread_t) * number_of_readers );
  indexes = (int*) malloc( sizeof(int) * number_of_readers );

  for(int i=0; i<number_of_readers; i++)
    indexes[i] = i;

  reader_item = (int**) malloc( sizeof(int*) * number_of_readers );
  for(int i=0; i<number_of_readers; i++)
    reader_item[i] = (int*) malloc( sizeof(int) * buffer_size );

  for(int i=0; i<number_of_readers; i++)
    for(int j=0; j<buffer_size; j++)
      reader_item[i][j] = 0;
}

void read_it(int reader_id, int item_id, int* unread)
{
  int old_cancel_state;
  pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, &old_cancel_state);
  printf("%d reads id = %d , item = %d\n", reader_id, item_id, buffer[item_id]);
  pthread_setcancelstate (old_cancel_state, NULL);
  reader_item[reader_id][item_id] = 1;
  items_unread[item_id]--;
  (*unread)--;
}

void* reader_thread(void* args)
{
  int idx = *(int*)args;
  int unread = number_of_items;
  while(unread)
  {
    int item_idx = rand()%buffer_size;
    pthread_mutex_lock(&mutex[item_idx]);
    if(buffer[item_idx]!=-1 && !reader_item[idx][item_idx])
      read_it(idx, item_idx, &unread);
    pthread_mutex_unlock(&mutex[item_idx]);
  }
  return NULL;
}

void insert_into_buffer(int idx, int * items_remainig)
{
  int old_cancel_state;
  buffer_type item = get_buffer_item();
  buffer[idx] = item;
  items_unread[idx] = number_of_readers;
  for(int i=0; i<number_of_readers; i++)
    reader_item[i][idx] = 0;
  (*items_remainig)--;
  pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, &old_cancel_state);
  printf("writer writes id = %d : item = %d\n", idx, item);
  pthread_setcancelstate (old_cancel_state, NULL);
}

void* writer_thread(void* no)
{
  int j, items_remainig = number_of_items;
  while(items_remainig)
  {
    for(int i=0; i<buffer_size; i++)
    {
      j = 0;
      pthread_mutex_lock(&mutex[i]);
      if(items_unread[i]==0)
      {
        insert_into_buffer(i, &items_remainig);
        j = 1;
      }
      pthread_mutex_unlock(&mutex[i]);
      if(j)break;
    }
  }

  return NULL;
}

void start_threads()
{
  for(int i=0; i<number_of_readers; i++)
    pthread_create(&readers[i], NULL, reader_thread, &indexes[i]);
  pthread_create( &writer, NULL, writer_thread, NULL);

  for(int i=0; i<number_of_readers; i++)
    pthread_join(readers[i], 0);
  pthread_join(writer, 0);
}

void cleanout()
{
  free(buffer);
  free(mutex);
  free(indexes);
  free(readers);
  free(items_unread);
  for(int i=0; i<number_of_readers; i++)
    free(reader_item[i]);
  free(reader_item);
}

int getNum(char * str)
{
  if(str==NULL)return -1;
  int a=0, i=0;
  while(1)
  {
    if(str[i]=='\0')break;
    if(str[i]<'0' || str[i]>'9')
    {
      a=-1;
      break;
    }
    a = a*10 + str[i]-'0';
    i++;
  }
  return a;
}

int main(int argc, char *argv[])
{
  if(argc != 4)
  {
    printf("give correct args.\n");
    printf("./a.out num_readers buffer_size number_of_items\n");
    return 0;
  }
  int numReaders = getNum(argv[1]);
  int buffSize = getNum(argv[2]);
  int numItems = getNum(argv[3]);

  if(numReaders<0 || buffSize<0 || numItems<0)
  {
    printf("give correct args.\n");
    printf("./a.out num_readers buffer_size number_of_items\n");
    return 0;
  }

  number_of_items = numItems;
  buffer_size = buffSize;
  number_of_readers = numReaders;
  initialize();
  start_threads();
  cleanout();
  return 0;
}
