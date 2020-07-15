#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define MAGIC 1234567

#define ALIGNMENT 2 * sizeof(size_t)

typedef struct Memchunk {
	size_t capacity;
	struct Memchunk *next;
	struct Memchunk *prev;
	bool free;
	int magic;
	void *payload;
} Memchunk;

typedef struct Fastchunk {
	int magic;
	struct Fastchunk *next;
	bool free;
	void *payload;
} Fastchunk;

typedef struct Heavychain {
	/* LIFO */
	struct Fastchunk *fast_bins[10];
	/* FIFO */
	struct Memchunk *slow_bins;
} Heavychain;

enum status_codes {
	OK,
	ERROR_NULL,
	ERROR_DOUBLE_FREE,
	ERROR_ALLOCATION,
};

int initialize();

void *mymalloc(size_t size);
void myfree(void *ptr, int *status);

Memchunk *allocate_chunk(size_t size);
Fastchunk *allocate_fastchunk(size_t size);

Fastchunk *find_first_fastchunk(Fastchunk *chunk);
Memchunk *find_best_fit(size_t size, Memchunk *chunk);
Memchunk *find_first_fit(size_t size);
Memchunk *find_worst_fit(size_t size);

bool is_fast_size(size_t size);
