#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <assert.h>

#include "mymalloc.h"

static Memchain *memchain = NULL;

void initialize(int *status) {
	*status = OK;
	if (memchain != NULL) {
		return;
	}

	if ((memchain = sbrk(sizeof(memchain))) == (void *) - 1) {
		*status = ERROR_ALLOCATION;
		return;
	}

	memchain->head = NULL;
	memchain->tail = NULL;
}

void *mymalloc(size_t size) {
	Memnode *node;
	int *status;

	if (!memchain) {
		initialize(status);
	}

	if (*status != OK) {
		return NULL;
	}

	if ((node = find_best_fit(size)) == NULL) {
		node->free = 0;

		if (node->capacity > sizeof(node) + size) {
			split_node(node, size);
		}

		return node->payload;
	}

	if ((node = allocate_node(size)) == NULL) {
		return NULL;
	}

	if (memchain->head == NULL) {
		memchain->head = node;
	}

	/* Update tail of chain to point to new block, and set
	 * new node as tail. */
	if (memchain->tail) {
		memchain->tail->next = node;
		node->prev = memchain->tail;
	}
	memchain->tail = node;

	return node->payload;
}

void myfree(void *ptr, int *status) {
	if (!ptr) {
		*status = ERROR_NULL;
	}

	// Access metadata
	Memnode *node = (Memnode *) ptr - 1;
	assert(node->magic = 1234567);

	if (node->free) {
		*status = ERROR_DOUBLE_FREE;
	}

	node->free = 1;

	if (node->next != NULL && node->next->free) {
		merge_next_node(node);
	}
	if (node->prev != NULL && node->prev->free) {
		merge_prev_node(node);
	}
}

Memnode *allocate_node(size_t size) {
	Memnode *node;

	size_t total_size = sizeof(Memnode) + size;

	if ((node = sbrk(total_size)) == (void *) - 1) {
		return NULL;
	}

	node->capacity = total_size - size;
	node->next = NULL;
	node->prev = NULL;
	node->free = 1;
	node->magic = 1234567;
	node->payload = node + 1;

	return node;
}

void split_node(Memnode *node, size_t size) {
}

Memnode *merge_next_node(Memnode *node) {
	Memnode *next_node = node->next;

	node->capacity = sizeof(Memnode) + next_node->capacity;

	if (next_node->next) {
		next_node->prev = node;
	}
	else {
		assert(memchain->tail == next_node);
		memchain->tail = node;
	}
}

Memnode *merge_prev_node(Memnode *node) {
	Memnode *prev_node = node->prev;

	prev_node->capacity = sizeof(Memnode) + node->capacity;
	prev_node->next = node->next;

	if (node->next) {
		node->next->prev = prev_node;
	}
	else {
		assert(memchain->tail == node);
		memchain->tail = prev_node;
	}
}

Memnode *find_best_fit(size_t size) {
	if (!memchain) {
		return NULL;
	}

	int min_delta = find_min_delta(size);
	if (min_delta == INT_MAX) {
		return NULL;
	}

	Memnode *curr = memchain->head;
	while (curr) {
		if (curr->free) {
			if (abs(curr->capacity - size) == min_delta) {
				return curr;
			}
		}

		curr = curr->next;
	}

	return NULL;
}

Memnode *find_first_fit(size_t size) {
	if (!memchain) {
		return NULL;
	}

	Memnode *curr = memchain->head;
	while (curr) {
		if (curr->free) {
			if (curr->capacity >= size) {
				return curr;
			}
		}

		curr = curr->next;
	}

	return NULL;
}

Memnode *find_worst_fit(size_t size) {
	if (!memchain) {
		return NULL;
	}

	int max_delta = find_max_delta(size);
	if (max_delta == INT_MIN) {
		return NULL;
	}

	Memnode *curr = memchain->head;
	while (curr) {
		if (curr->free) {
			if (curr->capacity == max_delta) {
				return curr;
			}
		}

		curr = curr->next;
	}

	return NULL;
}

int find_max_delta(size_t size) {
	Memnode *curr = memchain->head;

	int delta = INT_MIN;

	while (curr) {
		if (curr->free) {
			if (abs(curr->capacity - size) > delta) {
				delta = abs(curr->capacity - size);
			}
		}

		curr = curr->next;
	}

	return delta;
}


int find_min_delta(size_t size) {
	Memnode *curr = memchain->head;

	int delta = INT_MAX;

	while (curr) {
		if (curr->free) {
			if (abs(curr->capacity - size) < delta) {
				delta = abs(curr->capacity - size);
			}
		}

		curr = curr->next;
	}

	return delta;
}

void main() {
	return;
}
