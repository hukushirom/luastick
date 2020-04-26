#include "common.h"
#include <time.h>
#include "mba/hashmap.h"
#include "mba/suba.h"

#define AL(h) ((struct allocator *)((h)->al ? (char *)(h) - (ptrdiff_t)(h)->al : NULL))

int
HashmapExercise(int verbose, struct cfg *cfg, char *args[])
{
	struct hashmap *h;
    int rate = 0, i, n = 0;
	int count = atoi(args[0]);
	int interval = atoi(args[1]);
	iter_t riter;
	iter_t iter;
	char *str;
	struct allocator *al = NULL;
	char *mem;
	clock_t t0;
	int chkpnt = 0;
	struct allocator *hal;

	if ((mem = malloc(0xFFFFFF)) == NULL ||
			(al = suba_init(mem, 0xFFFFFF, 1, 0)) == NULL ||
			(h = hashmap_new(hash_str, cmp_str, NULL, al)) == NULL) {
		AMSG("");
		return -1;
	}
	hal = AL(h);

/*
	if ((h = hashmap_new(hash_str, cmp_str, NULL, al)) == NULL) {
		AMSG("");
		return -1;
	}
*/
srand(0);

	t0 = clock();
if (verbose)
fprintf(stderr, "\n       time    count     size      mem\n");

	hashmap_iterate(h, &iter);
	rate_iterate(&riter);
	for (i = 0; i < count; i++) {
		if ((i % interval) == 0) {
if (verbose)
fprintf(stderr, "%2d %8ld %8d %8d %8d\n", chkpnt++, (clock() - t0) / 1000, hashmap_size(h), table_sizes[h->table_size_index], hal->alloc_total - hal->free_total);
			rate = rate_next(&riter);
		}

		if (rand() % 10 < rate) {
	        str = allocator_alloc(NULL, 8, 0);
	        sprintf(str, "%07d", n++);
	        if (hashmap_put(h, str, str) == -1) {
				AMSG("");
				return -1;
			} else {
/*fputc('+', stderr);
*/
	 	      	tcase_printf(verbose, "put %s %d\r", str, hashmap_size(h));
			}
		} else {
			if (hashmap_is_empty(h)) {
/*fputc('0', stderr);
*/
				tcase_printf(verbose, "hashmap empty\r");
			} else {
		        str = hashmap_next(h, &iter);
				if (str) {
					char *data;

	    	    	tcase_printf(verbose, "get %s %d\r", str, hashmap_size(h));
					if (hashmap_remove(h, (void **)&str, (void **)&data) == -1) {
						AMSG("");
						return -1;
					}
/*fputc('-', stderr);
*/
					tcase_printf(verbose, "rem %s %d\r", str, hashmap_size(h));
					allocator_free(NULL, str);
				} else {
/*fputc('x', stderr);
*/
					tcase_printf(verbose, "nothing left to iterate over\r");
					hashmap_iterate(h, &iter);
				}
			}
		}
    }

if (verbose)
fprintf(stderr, "%2d %8ld %8d %8d %8d\n", chkpnt++, (clock() - t0) / 1000, hashmap_size(h), table_sizes[h->table_size_index], hal->alloc_total - hal->free_total);
	hashmap_del(h, allocator_free, NULL, NULL);
/*
	free(mem);
*/
if (verbose)
fprintf(stderr, "bytes outstanding from allocator: %d\n", hal->alloc_total - hal->free_total);

	cfg = NULL;
    return 0;
}
