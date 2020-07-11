#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define MAGIC 1234567

#define BEST_FIT 0
#define FIRST_FIT 1
#define WORST_FIT 2

#define ALIGNMENT 2 * sizeof(size_t)

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

typedef struct HeavyChain {
	/* LIFO lists of pre-established size nodes */
	struct Memnode fast_bins[10];
	struct Memnode *slow_bins;
} HeavyChain;

enum status_codes {
	OK,
	ERROR_NULL,
	ERROR_DOUBLE_FREE,
	ERROR_ALLOCATION,
};

int initialize();

void *mymalloc(size_t size);
void myfree(void *ptr, int *status);

Memnode *allocate_node(size_t size);
void split_node(Memnode *node, size_t size);
Memnode *merge_next_node(Memnode *node);
Memnode *merge_prev_node(Memnode *node);

int find_min_delta(size_t size);
int find_max_delta(size_t size);

Memnode *find_best_fit(size_t size);
Memnode *find_first_fit(size_t size);
Memnode *find_worst_fit(size_t size);
