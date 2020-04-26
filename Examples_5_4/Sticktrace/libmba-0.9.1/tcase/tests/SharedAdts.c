#include "common.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "mba/cfg.h"
#include "mba/suba.h"
#include "mba/stack.h"
#include "mba/linkedlist.h"
#include "mba/varray.h"
#include "mba/svsem.h"

static int
produce(struct allocator *suba, struct linkedlist *l, int n, svsem_t *mutex, svsem_t *empty, svsem_t *full)
{
	int i, w, emptyval, fullval;
	char *str;

	for (i = 0; i < n; i++) {
		if ((w = svsem_wait(empty)) == -1) {
			MMSG("");
			svsem_getvalue(empty, &emptyval);
			MMNF(errno, ": empty=%d", emptyval);
			return -1;
		}
		svsem_wait(mutex);

		if ((str = allocator_alloc(suba, 64, 0)) == NULL) {
			AMSG("");
			return -1;
		}

		svsem_getvalue(empty, &emptyval);
		svsem_getvalue(full, &fullval);
		sprintf(str, "i=%d,suba=%d,size=%d,empty=%d,full=%d,tot=%d,w=%d\n", i, suba->alloc_total - suba->free_total, linkedlist_size(l), emptyval, fullval, emptyval + fullval, w);

		if (linkedlist_add(l, str) == -1) {
			AMSG("");
			return -1;
		}

/*
		fputs("p ", stderr);
		fputs(str, stderr);
*/
		svsem_post(mutex);
		svsem_post(full);
	}

	return 0;
}
static int
consume(struct allocator *suba, struct linkedlist *l, int n, svsem_t *mutex, svsem_t *empty, svsem_t *full)
{
	int i, emptyval, fullval;
	char *str, buf[1024];

	for (i = 0; i < n; i++) {
		svsem_wait(full);
		svsem_wait(mutex);

		if ((str = linkedlist_remove(l, 0)) == NULL) {
			AMSG("");
			return -1;
		}

		svsem_getvalue(empty, &emptyval);
		svsem_getvalue(full, &fullval);
		sprintf(buf, "c i=%d,suba=%d,size=%d,empty=%d,full=%d,tot=%d", i, suba->alloc_total - suba->free_total, linkedlist_size(l), emptyval, fullval, emptyval + fullval);
/*
puts(buf);
*/
		allocator_free(suba, str);

		svsem_post(mutex);
		svsem_post(empty);
	}

	return 0;
}

int
SharedAdts(int verbose, struct cfg *cfg, char *args[])
{
	struct allocator *suba;
	struct linkedlist *l;
	size_t memsiz = atoi(args[0]);
	int n = atoi(args[1]);
	void *mem;
	int pid, status, ret = 0;
	svsem_t mutex, empty, full;

	errno = 0;
	set_signals();

	if ((mem = open_mmap("SharedAdts", O_RDWR | O_CREAT, 0600, memsiz)) == NULL) {
		AMSG("");
		return -1;
	}
	if (1) {
		if ((suba = suba_init(mem, memsiz, 1, 0)) == NULL) {
			AMSG("");
			return -1;
		}
	} else {
		suba = NULL;
	}

	if ((l = linkedlist_new(0, suba)) == NULL) {
		AMSG("");
		ret = -1;
		goto err;
	}

	if (svsem_create(&mutex, 1, 1) == -1) {
		AMSG("");
		ret = -1;
		goto err;
	}
	if (svsem_create(&empty, n, 0) == -1) {
		AMSG("");
		ret = -1;
		goto err;
	}
	if (svsem_create(&full, 0, 0) == -1) {
		AMSG("");
		ret = -1;
		goto err;
	}

	if ((pid = fork()) == 0) { /* child */
		consume(suba, l, n * 1000, &mutex, &empty, &full);
		munmap(mem, memsiz);
		exit(ret);
	}

	if (produce(suba, l, n * 1000, &mutex, &empty, &full)) {
		AMSG("");
		ret = -1;
		goto err;
	}

	waitpid(pid, &status, 0);
	if ((errno = WEXITSTATUS(status))) {
		PMNO(errno);
		ret = -1;
		goto err;
	}

	ret += svsem_destroy(&full);
	ret += svsem_destroy(&empty);
	ret += svsem_destroy(&mutex);
	if (ret) {
		AMSG("");
		ret = -1;
		goto err;
	}

	tcase_printf(verbose, "done ");
err:
	linkedlist_del(l, NULL, NULL);
/*printf("alloc_total: %d free_total: %d\n", suba->alloc_total, suba->free_total);
 */
	munmap(mem, memsiz);

	cfg = NULL;
    return ret;
}

