#include <stdlib.h>
#include <stdio.h>
#include <unisted.h>
#include <limits.h>
#include <assert.h>

#include "mymalloc.h"

static HeavyChain *heavychain = NULL;

int initialize() {
	int status = OK;
	if (heavychain) {
		return status;
	}
	if ((heavychain = sbrk(sizeof(HeavyChain))) == (void *) - 1) {
		status = ERROR_ALLOCATION;
		return status;
	}

	int fast_bin_sizes[10] = {
		16; 24; 32; 40; 48;
		56; 64; 72; 80; 88;
	};

	for (int i = 0; i < 10; i++) {
		fast_bins[i] = allocate_node(fast_bin_sizes[i]);	
	}
}

bool is_fast_size(size_t size) {
	return ((8 < size <= 88) && (size % 8 == 0));
}

Memnode *find_first_fastbin(Memnode *curr) {
	while (curr) {
		if (curr->free) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

void *mymalloc(size_t size) {
	Memnode *result;

	if (!heavychain) {
		if (initialize() != OK) {
			return NULL;
		}
	}

	/* Search the fast bins */
	if (is_fast_size(size)) { 
		int index = (size % 8) - 2;
		if (result = find_first_fastbin(fast_bins[index])) {
			return result->payload;
		}
		/* No free node in the bin, create new node and prepend */
		else {
			if ((result = allocate_node(size)) == NULL) {
				return NULL;
			}	

			/* New node allocation and initialization.
			 * New node becomes head of the LIFO bin list. */
			result->free = 0;
			result->next = fast_bins[index];
			result->prev = NULL;
			result->next->prev = result;
			fast_bins[index] = result;

			return result->payload;
		}
	}
}
