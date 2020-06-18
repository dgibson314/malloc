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

void test_reuse_malloc() {
	int *ptr1 = mymalloc(sizeof(int));
	*ptr1 = 1;
	
	int status;
	myfree(ptr1, &status);

	int *ptr2 = mymalloc(sizeof(int));
	*ptr2 = 2;

	assert(ptr1 == ptr2);
	myfree(ptr2, &status);
}

void print_simple_malloc() {
	int *ptr = mymalloc(sizeof(int));

	Memnode *meta = (Memnode *) ptr - 1;

	assert(meta->magic == MAGIC);

	printf("Node: \n");
	printf("Capacity: %ld\n", meta->capacity);
	printf("Free: %d\n", meta->free);
}
