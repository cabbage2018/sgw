//#ifdef _MSC_VER
//#pragma once
//#endif // _MSC_VER
typedef struct list * _value;

// one bit
typedef struct coil {
  bool *bl;
}_coil;

//16 bits
typedef struct rgst {
  uint16_t *u16;
}_rgst;

#define MAX_KEY_LENG 145

typedef struct cell{
  char *key;
  struct list *value;
}_cell;

#define LIST_LENGTH_MAX 65535

int lol_init(struct list** List);

//keep order of input
iterator lol_insert(struct list* List, void *data, iterator itBefore, void (*assign)(void*, const void*));

int lol_findFirst(const char *key, struct list** outlet, int(*equal)(const void*, const void*));

int lol_remove(const struct list* outlet);

bool lol_isEmpty();

int lol_destroy();