#include "common.h"
#include <time.h>
#include "mba/linkedlist.h"
#include "mba/time.h"
#include "mba/suba.h"

#define randint(a,b) (a)+(((b)-(a)+1)*(float)rand()/RAND_MAX)

int suba_print_free_list(struct allocator *suba);

int
SubaExercise(int verbose, struct cfg *cfg, char *args[])
{
    int ret, rate, i, min = atoi(args[2]), max = atoi(args[3]);
	struct linkedlist *l;
	void *mem, *ptr;
	struct allocator *suba;
	size_t memsiz = atoi(args[0]);
    size_t mincell = atoi(args[1]);
	uint64_t t0;

int r = atoi(args[4]);
srand(r);

	cfg = NULL;
	ret = 0;

	l = linkedlist_new(0, NULL);
	if ((mem = open_mmap("suba", O_RDWR | O_CREAT, 0600, memsiz)) == NULL) {
		AMSG("");
		return -1;
	}
	if (1) {
		if ((suba = suba_init(mem, memsiz, 1, mincell)) == NULL) {
			AMSG("");
			return -1;
		}
	} else {
		suba = NULL;
	}

t0 = time_current_millis();
	rate = EXERCISE_R0;
	for (i = 0; i < EXERCISE_MED_COUNT; i++) {
		if (i == EXERCISE_MED_P1) {
			rate = EXERCISE_R1;
		} else if (i == EXERCISE_MED_P2) {
			rate = EXERCISE_R2;
		} else if (i == EXERCISE_MED_P3) {
			rate = EXERCISE_R3;
		}

		if (rand() % 10 < rate) {
			char op = '+';
			size_t size;
			size = randint(min,max);
			size = size & 0xFFFFEED; /* quantized sizes a little more realistic */
			if (rand() % 10 < 1 && (ptr = linkedlist_remove(l, randint(0, linkedlist_size(l))))) {
				op = size ? 'r' : '-';
				ptr = allocator_realloc(suba, ptr, size);
			} else {
				ptr = allocator_alloc(suba, size, 0);
			}
			if (size) {
				if (!ptr) {
					AMSG("");
					ret = -1;
					goto err;
				}
				memset(ptr, 'x', size);
				linkedlist_add(l, ptr);
			}
if (verbose == 2)
fputc(op, stderr);
		} else {
			int idx = linkedlist_size(l);
			if (idx) {
				idx = randint(0, idx - 1);
			}
			ptr = linkedlist_remove(l, idx);
			if (allocator_free(suba, ptr) == -1) {
				AMSG("");
				ret = -1;
				goto err;
			}
if (verbose == 2)
fputc(ptr ? '-' : 'x', stderr);
		}
    }
if (verbose > 2) {
double s = suba->size_total;
double a = suba->alloc_total;

fprintf(stderr, "%8llu %8u %8u %0.3f ",
		time_current_millis() - t0,
		suba->size_total,
		suba->alloc_total,
		s / a * 100.0);
}
err:
	munmap(mem, memsiz);
	linkedlist_del(l, NULL, NULL);

	return ret;
}

