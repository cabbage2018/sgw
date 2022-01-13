//#ifdef _MSC_VER
//#pragma once
//#endif // _MSC_VER

typedef struct node{
  void * data;
  struct node *next;
  struct node *last;
}_node;

struct list{
  struct node *head;
  int memsize;
  int count;
}_list;

enum CODE{
  CODE_OK = 0,
  NULL_EXCEPTION =1,
  LIST_LIMIT_EXCEEDED,
  LIST_OUT_OF_MEMORY,
};

typedef struct node** iterator;

#define LIST_LENGTH_MAX 65535
// typedef struct list* List;

int init(struct list** List);

// append data to tail
iterator append(struct list* List, void *data, void (*assign)(void*, const void*));

iterator insert(struct list* List, void *data, iterator itBefore, void (*assign)(void*, const void*));
// without Release memory
iterator remove2(struct list* List, iterator it);

int removeFirst(struct list* List);

int removeLast(struct list* List);

// index from 0
void* at(struct list* List, int index);

iterator findFirst(struct list* List, void* data, int(*condition)(const void*, const void*));

iterator indexOf(struct list* List, void* data, int(*equal)(const void*, const void*));

int len();

int isEmpty();

// void destroy();

iterator begin(struct list* List);
iterator end(struct list* List);
iterator getNext(iterator it);
iterator getLast(iterator it);

