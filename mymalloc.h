#define MAGIC 1234567

struct Memnode {
	size_t size;
	size_t capacity;
	Memnode *next;
	Memnode *prev;
	bool free;
	int magic;
	void *payload;
} Memnode;

struct Memchain {
	Memnode *head;
	Memnode *tail;
}

void *mymalloc(size_t size);
void *myfree(void *ptr);

int find_min_delta(size_t size);

Memnode *find_best_fit(size_t size);

Memnode *merge_next_node(Memnode *node);
Memnode *merge_prev_node(Memnode *node);

