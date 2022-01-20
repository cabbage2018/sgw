#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "list.h"

int init(struct list** List/*, int memSize*/){
  struct list *l = malloc(sizeof(struct list));
  //ASSERT_NOT_NULL(List, code);
  l->count = 0;
  l->head = NULL;
  l->memsize = sizeof(struct list);
  *List = l;
}

// not possible when do this in geneirc template manner

// void destroy(struct list* List, void (*myfree)(void*)) {
//   while(List->head) {
//     struct node *p = List->head;
//     removeFirst(List, &p);
//     myfree(p->data);
//   }
// }

// append data to tail
iterator append(struct list* List, void *data, void (*assign)(void*, const void*)) {
  struct node *new_node = (struct node *)malloc(sizeof(struct node));
  // enum CODE code = CODE_OK;
  // MCL_ASSERT_CODE_MESSAGE(List->count < LIST_LENGTH_MAX, LIST_LIMIT_EXCEEDED, "Index of the list is already at the maximum value. Not adding the new data!");
  // MCL_NEW(new_node);
  // MCL_ASSERT_CODE_MESSAGE(NULL != new_node, LIST_OUT_OF_MEMORY, "Not enough memory to allocate new mcl_node!");
  (*assign)(new_node->data, (const void*)data);
  // new_node->data = data;
  new_node->last = new_node;
  new_node->next = new_node;
  // printf("new_node initialized and its data assigned to the received one. Node's data address = <%p>", (void *) new_node->data);

  if (NULL == List->head) {
    List->head = new_node;
  } else {
    new_node->last = List->head->last;
    new_node->next = List->head;
    List->head = new_node;
    }
  List->count++;
  List->memsize += sizeof(new_node);
  return &new_node;
}

iterator insert(struct list* List, void *data, iterator itBefore, void (*assign)(void*, const void*)) {
  struct node *new_node = (struct node *)malloc(sizeof(struct node));
  (*assign)(new_node->data, data);
  new_node->last = new_node;
  new_node->next = new_node;
  printf("new_node initialized and its data assigned to the received one. Node's data address = <%p>", (void *) new_node->data);

  if (NULL == List->head) {
    printf("This is the first node in the list.");
    List->head = new_node;
  } else {
    new_node->last = List->head->last;
    new_node->next = List->head;
    List->head = new_node;
  }
  List->count++;
  List->memsize += sizeof(new_node);
  return (struct node**)&new_node;
}

iterator remove2(struct list* List, iterator it) {
  iterator moved = NULL;
  if(List->head) {
    // not empty
    if(List->head->next == List->head && List->head->next == (struct node*)(*it)) {
      // only one node
      (*moved) = *it;
      (struct node*)(*it)->last = NULL;
      (struct node*)(*it)->next = NULL;
      
      List->head = NULL;
      List->count = 0;
      List->memsize -= sizeof(*it);
    } else {
      // >1
      struct node *p = List->head;
      for(int i = 0; i < List->count; i ++) {
        if((struct node*)*it == p) {

          (*moved) = *it;
          (*it)->last->next = (*it)->next;
          (*it)->next->last = (*it)->last;

          (*it)->next = NULL;
          (*it)->last = NULL;

          List->count --;
          List->memsize -= sizeof(*it);

          break;
        }
        p = p->next;
      }
    }
  }
  return (struct node*)*moved;
}

int removeFirst(struct list* List, iterator it) {
  int result = 0;
  if(List->head) {
    // not empty
    if(List->head->next == List->head) {
      // only one node
      (*it) = List->head;// &List->head;
      List->head = NULL;
      List->count = 0;
    } else {
      // >1
      struct node *tail = List->head->last;
      struct node *head2 = List->head->next;
      head2->last = tail;
      tail->last = head2;

      struct node *removed = List->head;
      removed->next = NULL;
      removed->last = NULL;

      List->head = head2;
      List->count -= 1;
      List->memsize -= sizeof(struct node);
    }
    result += 1;
  }
  return result;
}

int removeLast(struct list* List, iterator it) {
  int result = 0;
  if(List->head) {
    // not empty
    if(List->head->next == List->head) {
      // only one node
      (*it) = List->head;
      List->head = NULL;
      List->count = 0;
    } else {
      // >1, re-arrage the layout
      struct node *tail2 = List->head->last->last;
      struct node *head = List->head;
      List->head->last = tail2;
      tail2->next = List->head;

      struct node *removed = List->head->last;
      removed->next = NULL;
      removed->last = NULL;

      List->count -= 1;
      List->memsize -= sizeof(struct node);
    }
    result += 1;
  }
  return result;
}

// index from 0
void* at(struct list* List, int index) {
  if(index > List->count - 1) {
    return NULL;
  } else {
    int skip = index;
    struct node *cur = List->head;
    while(skip) {
      cur = cur->next;
      skip -= 1;
    }
    return cur;
  }
}

// condition() 
//    return 1 when first arg < second arg
//    return 0 otherwise...
iterator findFirst(struct list* List, void* data, int(*condition)(const void*, const void*)) {  
  if(List->head) {
    struct node *p = List->head;
    do {            
      if((condition(data, p->data) == 0 && condition(p->data, data) == 0)) {
        return &p;
      }
      p = p->next;
    } while(p != List->head);
    printf("Cannot find any matched element.");
  }
  printf("The doublely linked list doet not have any element.");
  return NULL;
}

// equal return 1 when equal, 0 otherwise?
iterator indexOf(struct list* List, void* data, int(*equal)(const void*, const void*)) {
  int total = List->count -1;
  struct node *p = List->head;
  do {
    if(equal(data, p->data) == 1) {
      return &p;
    }
    p = p->next;
  } while(total--);
  printf("List node NOT found.");
  return NULL;
}

int len(struct list* List) {
  return List->count;
}

int isEmpty(struct list* List) {
  bool result = (NULL == List->head) ? true : false;
  return result;
}

iterator begin(struct list* List) {
  return & List->head;
}

iterator end(struct list* List) {
  return & List->head->last;
}

iterator getNext(struct list* List, iterator it) {
  if(it && (struct node*)(*it)->next) {
	  if (*it == List->head) {
		  return NULL;
	  } else {
		  return &((*it)->next);
	  }
  } else {
    printf("Input is NULL!");
    return NULL;
  }
}

iterator getLast(struct list* List, iterator it) {
  if(it && (struct node*)(*it)->last) {
    return (struct node**) &(*it)->last;
  } else {
    printf("Input is NULL!");
    return NULL;
  }
}