#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define MAGIC 1234567

typedef struct Memnode {
	size_t capacity;
	struct Memnode *next;
	struct Memnode *prev;
	bool free;
	int magic;
	void *payload;
} Memnode;

typedef struct Memchain {
	struct Memnode *head;
	struct Memnode *tail;
} Memchain;

void initialize(int *status);

void *mymalloc(size_t size);
void *myfree(void *ptr);

Memnode *allocate_node(size_t size);

int find_min_delta(size_t size);

Memnode *find_best_fit(size_t size);
Memnode *find_first_fit(size_t size);
Memnode *find_worst_fit(size_t size);

Memnode *merge_next_node(Memnode *node);
Memnode *merge_prev_node(Memnode *node);

