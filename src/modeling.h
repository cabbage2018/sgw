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

// from static file
int buildTable(struct list* List, void *data, iterator itBefore, void (*assign)(void*, const void*));

// resp is Json String
int resp2List(const char *key, struct list** outlet, int(*equal)(const void*, const void*));

// look up the Json table to parse resp
int lookupVars(const struct list* outlet);

// 
int presentVars();