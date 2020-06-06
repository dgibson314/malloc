#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "mymalloc.h"

void test_simple_malloc(void);
void print_simple_malloc(void);

void main() {
	test_simple_malloc();
}

void test_simple_malloc() {
	int *ptr = mymalloc(sizeof(int));

	Memnode *meta = (Memnode *) ptr - 1;

	assert(meta->magic == MAGIC);
	assert(meta->capacity == sizeof(int));
	assert(meta->free == 0);
	assert(!meta->next);
	assert(!meta->prev);

	int status;
	myfree(ptr, &status);
}

void print_simple_malloc() {
	int *ptr = mymalloc(sizeof(int));

	Memnode *meta = (Memnode *) ptr - 1;

	assert(meta->magic == MAGIC);

	printf("Node: \n");
	printf("Capacity: %ld\n", meta->capacity);
	printf("Free: %d\n", meta->free);
}
