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
}

