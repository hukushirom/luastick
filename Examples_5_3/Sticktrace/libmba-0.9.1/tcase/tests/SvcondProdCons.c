#include "common.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "mba/svsem.h"
#include "mba/svcond.h"
#include "mba/suba.h"

struct obj {
	unsigned long value;
	unsigned long limit;
	svsem_t *lock;
	svcond_t empty;
	svcond_t full;
};

static int
produce(struct obj *obj, unsigned long n)
{
	unsigned long i, stall;

	for (i = 0; i < n; i++) {
		if (svsem_wait(obj->lock) == -1) {
			AMSG("");
			return -1;
		}

		if (obj->value == obj->limit) {
			if (svcond_wait(&obj->full, obj->lock) == -1) {
				AMSG("");
				return -1;
			}
		}

		obj->value++;

		if (svcond_signal(&obj->empty) == -1) {
			AMSG("");
			return -1;
		}

		if (svsem_post(obj->lock) == -1) {
			AMSG("");
			return -1;
		}
stall = 10000;
while (stall--);
	}

	return 0;
}
static int
consume(struct obj *obj, unsigned long n)
{
	unsigned long i, stall;

	for (i = 0; i < n; i++) {
		if (svsem_wait(obj->lock) == -1) {
			AMSG("");
			return -1;
		}

		if (obj->value == 0) {
			if (svcond_wait(&obj->empty, obj->lock) == -1) {
				AMSG("");
				return -1;
			}
		}

		obj->value--;

		if (svcond_signal(&obj->full) == -1) {
			AMSG("");
			return -1;
		}

		if (svsem_post(obj->lock) == -1) {
			AMSG("");
			return -1;
		}
stall = 1000;
while (stall--);
	}

	return 0;
}

int
SvcondProdCons(int verbose, struct cfg *cfg, char *args[])
{
	struct allocator *al;
	size_t memsiz = atoi(args[0]);
	unsigned long limit = atoi(args[1]);
	unsigned long n = atoi(args[2]);
	void *mem;
	int pid, status, ret = 0;
	struct pool sempool;
	struct obj *obj;

	errno = 0;
	set_signals();

	if ((mem = open_mmap("SvcondProdCons", O_RDWR | O_CREAT, 0600, memsiz)) == NULL) {
		AMSG("");
		return -1;
	}
	if ((al = suba_init(mem, memsiz, 1, 0)) == NULL) {
		AMSG("");
		ret = -1;
		goto err;
	}
	if (svsem_pool_create(&sempool, 10, 1, 0, al) == -1) {
		AMSG("");
		ret = -1;
		goto err;
	}
	if ((obj = allocator_alloc(al, sizeof *obj, 1)) == NULL) {
		AMSG("");
		ret = -1;
		goto err;
	}

	obj->value = 0;
	obj->limit = limit;
	if ((obj->lock = pool_get(&sempool)) == NULL) {
		AMSG("");
		ret = -1;
		goto err;
	}
	if (svcond_create(&obj->empty, &sempool) == -1 ||
			svcond_create(&obj->full, &sempool) == -1) {
		AMSG("");
		ret = -1;
		goto err;
	}

	if ((pid = fork()) == 0) { /* child */
		if (consume(obj, n) == -1) {
			AMSG("");
			return -1;
		}
		svcond_signal(&obj->full);
		exit(0);
	}

	if (produce(obj, n)) {
		AMSG("");
		return -1;
		ret = -1;
		goto err;
	}
	svcond_signal(&obj->empty);

	waitpid(pid, &status, 0);
	if ((errno = WEXITSTATUS(status))) {
		PMNO(errno);
		ret = -1;
		goto err;
	}

	ret += svcond_destroy(&obj->empty);
	ret += svcond_destroy(&obj->full);
	ret += svsem_pool_destroy(&sempool);

	tcase_printf(verbose, "done ");
err:
	munmap(mem, memsiz);

	cfg = NULL;
    return ret ? -1 : 0;
}

