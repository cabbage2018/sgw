#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "list.h"

struct wrapdata {
	char key[64];
	int len;
	char *value;
};

void assign(void *d1, const void *d2) {
	struct wrapdata *r1 = (struct wrapdata*)d1;
	struct wrapdata *r2 = (struct wrapdata*)d2;

	strncpy(r1->key, 64, r2->key);
	r1->len = r2->len;
	memcpy(r1->value, 64, r2->value);
}

int condition(const void *d1, const void *d2) {
	struct wrapdata *r1 = (struct wrapdata*)d1;
	struct wrapdata *r2 = (struct wrapdata*)d2;
	return strcmp(r1->value, r2->value) < 0? 1 : 0;
	return r1->value < r2->value ? 1 : 0;
}

void equal(void *d1, const void *d2) {
	struct wrapdata *r1 = (struct wrapdata*)d1;
	struct wrapdata *r2 = (struct wrapdata*)d2;
	return strcmp(r1->value, r2->value) == 0 ? 1 : 0;
	return r1->value == r2->value ? 1 : 0;
}

void traverse() {
	struct list* List = NULL;
	int code = init(&List);
	
	struct wrapdata *w = malloc(sizeof(struct wrapdata));
	strncpy(w->key, 32, "key-for-node");
	w->len = sizeof(w->key) + sizeof(w->len) + sizeof(w->value);
	w->value = NULL;

	strncpy(w->key, 32, "key-for-node2");
	iterator result = insert(List, (void *)w, List->head, assign);
	
	iterator cur = &List->head;

	for (int index = 0; (0 == code) && (index < List->count); ++index) {
		printf("%p", ((struct wrapdata*)((*cur)->data))->value);
		code = getNext(cur);
	}
}

void flicker() {
	// init
	struct list* List = NULL;
	int code = init(&List);

	// stuff
	struct wrapdata *w = malloc(sizeof(struct wrapdata));
	strncpy(w->key, 32, "key-for-node");
	w->len = sizeof(w->key) + sizeof(w->len) + sizeof(w->value);
	w->value = NULL;
	code = insert(List, (void *)w, List->head, assign);

	struct wrapdata *v = malloc(sizeof(struct wrapdata));
	strncpy(v->key, 32, "key-for-node");
	v->len = sizeof(v->key) + sizeof(v->len) + sizeof(v->value);
	v->value = NULL;
	code = insert(List, (void *)v, List->head, assign);
	
	struct node *cur = List->head;
	
	// walk through
	for (int index = 0; (0 == code) && (index < List->count); ++index) {
		printf("%p", ((struct wrapdata*)(cur->data))->value);
		code = getNext(&cur);
	}

	// free

}
