#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <assert.h>

#include "heavymalloc.h"

static Heavychain *heavychain = NULL;

int initialize() {
	int status = OK;
	if (heavychain) {
		return status;
	}
	if ((heavychain = sbrk(sizeof(Heavychain))) == (void *) - 1) {
		status = ERROR_ALLOCATION;
		return status;
	}

	int fast_bin_sizes[10] = {
		16, 24, 32, 40, 48,
		56, 64, 72, 80, 88,
	};

	for (int i = 0; i < 10; i++) {
		heavychain->fast_bins[i] = allocate_fastchunk(fast_bin_sizes[i]);	
	}
}

Memchunk *allocate_chunk(size_t size) {
	Memchunk *chunk;

	size_t total_size = sizeof(Memchunk) + size;

	if ((chunk = sbrk(total_size)) == (void *) - 1) {
		return NULL;
	}

	chunk->capacity = total_size - sizeof(Memchunk);;
	chunk->next = NULL;
	chunk->prev = NULL;
	chunk->free = 1;
	chunk->magic = MAGIC;
	chunk->payload = chunk + 1;

	return chunk;
}

Fastchunk *allocate_fastchunk(size_t size) {
	Fastchunk *chunk;
	size_t total_size = sizeof(Fastchunk) + size;

	if ((chunk = sbrk(total_size)) == (void *) - 1) {
		return NULL;
	}

	chunk->free = 1;
	chunk->next = NULL;
	chunk->magic = MAGIC;
	chunk->payload = chunk + 1;

	return chunk;
}


bool is_fast_size(size_t size) {
	return ((8 < size <= 88) && (size % 8 == 0));
}

Fastchunk *find_first_fastchunk(Fastchunk *curr) {
	while (curr) {
		if (curr->free) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

void *mymalloc(size_t size) {
	if (!heavychain) {
		if (initialize() != OK) {
			return NULL;
		}
	}

	/* Search the fast bins */
	if (is_fast_size(size)) { 
		Fastchunk *fastresult;

		int index = (size % 8) - 2;
		if (fastresult = find_first_fastchunk(heavychain->fast_bins[index])) {
			return fastresult->payload;
		}
		/* No free node in the bin, create new node and prepend */
		else {
			if ((fastresult = allocate_fastchunk(size)) == NULL) {
				return NULL;
			}	

			/* New chunk allocation and initialization.
			 * New chunk becomes head of the LIFO bin list. */
			fastresult->free = 0;
			fastresult->next = heavychain->fast_bins[index];
			heavychain->fast_bins[index] = fastresult;

			return fastresult->payload;
		}
	}

	/* Search slow bins */
	else {
		Memchunk *slowresult = find_best_fit(size, heavychain->slow_bins);

		/* Chunk found in slow bins */
		if (slowresult) {
			slowresult->free = 0;

			/* TODO: chunk splitting */
			return slowresult->payload;
		}
		/* No suitable chunk found; allocate new slow chunk */
		if ((slowresult = allocate_chunk(size)) == NULL) {
			return NULL;
		}

		/* New chunk becomes head of FIFO slow list */
		slowresult->next = heavychain->slow_bins;
		slowresult->prev = NULL;
		slowresult->next->prev = slowresult;
		slowresult->free = 0;

		return slowresult->payload;
	}
}

void myfree(void *ptr, int *status) {
	if (!ptr) {
		*status = ERROR_NULL;
	}
}

Memchunk *find_best_fit(size_t size, Memchunk *curr) {
	Memchunk *result = curr;
	int min_delta = INT_MAX;

	while (curr) {
		if (curr->free) {
			int delta = curr->capacity - size;
			if (0 <= delta < min_delta) {
				min_delta = delta;
				result = curr;
			}
		}

		curr = curr->next;
	}

	return result;
}
