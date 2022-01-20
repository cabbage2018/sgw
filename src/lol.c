#ifndef _MSC_VER
#include <stdint.h>
#include <unistd.h>
#else
#include "stdint.h"
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "list.h"
#include "lol.h"

int lol_init(struct list** List) {
  struct list *l = malloc(sizeof(struct list));
  l->count = 0;
  l->head = NULL;
  l->memsize = sizeof(struct list);
  *List = l;

  printf("lol_init mem:%d", l->memsize);

  return l->memsize;
}

void *_assign_struct(void *des, const void *src) {
  

  return;
}

// hopefully original cell is NULL so the destroy step will be skipped
void lol_assign(void *d1, const void *d2) {
  struct list*r1 = (struct list*)d1;
  struct list*r2 = (struct list*)d2;
  size_t cons = 0;

  while (r1->head ) {
    struct node *t1 = r1->head;

    if (r1->head == r1->head->next) {
      // last node
      r1->head = NULL;
    } else {
      r1->head = r1->head->next;
    }

    free(t1);
    r1->count--;
    r1->memsize -= sizeof(struct node);
  }

  for (struct node* x1 = r2->head; x1->next != r2->head; x1 = x1->next) {
    struct node* y1 = malloc(sizeof(struct node));
    cons += sizeof(struct node);
    printf("consumed memory accumulated now", cons);
    memcpy(y1, sizeof(struct node), x1); // note: the node could not own any pointer at this time this level
    insert(r1, (void*) y1, NULL, _assign_struct);
  }
}

//keep order of input
iterator lol_insert(struct list* List, void *data, iterator itPrevious, void(*assign)(void*, const void*)) {
  struct node *x = malloc(sizeof(struct node));

  x->data = (struct cell *)data;
  
  x->next = (*itPrevious)->next;
  x->last = (*itPrevious);
  
  (*itPrevious)->next->last = x;
  (*itPrevious)->next = x;
}

int lol_equal(const void* d1, const void *d2) {
  struct cell*c1 = (struct cell*)d1;
  struct cell*c2 = (struct cell*)d2;
  size_t cons = 0;
  return strcmp(c1->key, c2->key) ==0? 1: 0;
}

int lol_findFirst(const struct list* List, const struct cell *target, struct node** outlet, int(*equal)(const void*, const void*)) {
  *outlet = NULL;
  int x = 0;
  for (struct node* x1 = List->head; x1->next != List->head; x1 = x1->next) {
    struct cell*sel = (struct cell*)x1->data;
    if ((*equal)(target, sel)) {
      printf("goal 's layout:%p", target);
      *outlet = x1;
    }
    x++;
  }

  if (*outlet == NULL) {
    printf("goal 's layout:%p is not found.", target);
  }
  return x;
}

int lol_remove(struct list* List, const struct node **target) {
  printf("layout@:%p in area %d", *target, List->memsize);

  // if target is not in this List then must be error
  // ...

  if (List->head == (*target)) {
    if (List->count == 1) {
      List->head = NULL;
    }
    else {
      struct node* t = *target;
      List->head = t->next;

      t->next->last = t->last;
      t->last->next = t->next;
    }
  }
  else {
    struct node* t = *target;

    t->next->last = t->last;
    t->last->next = t->next;
  }

  List->count--;
  List->memsize -= sizeof(struct node);
  return sizeof(struct node);
}

bool lol_isEmpty(struct list** List) {
  return (*List)->count == 0? true: false;
}

int lol_destroy(struct list** List) {
  while ((*List)->count > 0) {
    struct node* cur = (*List)->head;

    // break chain
    if (cur->last == (*List)->head) {
      // only 1?
      (*List)->head = NULL;
      --(*List)->count;
      //release mem
      printf("last block before destroy *node: %p", *List);
      free(cur);
      printf("last block after destroy *node: %p", *List);

      (*List)->memsize -= sizeof(struct node);
    }
    else {
      (*List)->head = cur->next;
      cur->last->next = cur->next;

      cur->next->last = cur->last;

      --(*List)->count;
      //release mem
      printf("mem before destroy *node: %p", *List);
      free(cur);
      printf("mem after destroy *node: %p", *List);

      (*List)->memsize -= sizeof(struct node);
    }
  }
  if ((*List)->count ==0 && (*List)->head == NULL) {
    printf("mem size before destroy *List: %d, %p", (*List)->memsize, *List);
    free(*List);
    printf("mem location after destroy *List: %p", *List);
  }
}



// condition() 
//    return 1 when first arg < second arg
//    return 0 otherwise...
// equal return 1 when equal, 0 otherwise?
