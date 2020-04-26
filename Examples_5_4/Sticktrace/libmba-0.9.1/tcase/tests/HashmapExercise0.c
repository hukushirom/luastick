#include "common.h"
#include <time.h>
#include "mba/suba.h"
#include "mba/hashmap0.h"

int
HashmapExercise0(int verbose, struct cfg *cfg, char *args[])
{
	struct hashmap0 *h;
    int rate = 0, i, n = 0;
	int count = atoi(args[0]);
	int interval = atoi(args[1]);
	iter_t riter;
	char *str;
	clock_t t0;
	void *mem;
	struct allocator *al;
	iter_t iter;
	int chkpnt = 0;

	if ((mem = malloc(0xFFFFFFF)) == NULL ||
			(al = suba_init(mem, 0xFFFFFFF, 1, 0)) == NULL ||
    		(h = hashmap0_new(HASHMAP_LARGE, hash_str,
			(compare_fn)strcmp, NULL, allocator_free, NULL, al)) == NULL) {
		AMSG("");
		return -1;
	}
srand(1);

fputc('\n', stderr);

	t0 = clock();
fprintf(stderr, "       time    count     size      mem\n");

	hashmap0_iterate(h, &iter);
	rate_iterate(&riter);
	for (i = 0; i < count; i++) {
		if ((i % interval) == 0) {
fprintf(stderr, "%2d %8ld %8d %8d %8d %d\n", chkpnt++, (clock() - t0) / 1000, hashmap0_size(h), HASHMAP_LARGE, h->al->size_total - h->al->free_total, rate);
			rate = rate_next(&riter);
		}

		if (rand() % 10 < rate) {
	        str = allocator_alloc(NULL, 16, 0);
	        sprintf(str, "%015d", n++);
	        if (hashmap0_put(h, str, str) == -1) {
				AMSG("");
				return -1;
			} else {
/*fputc('+', stderr);
*/
	 	      	tcase_printf(verbose, "put  %s %d\n", str, hashmap0_size(h));
			}
		} else {
			if (hashmap0_is_empty(h)) {
				tcase_printf(verbose, "hashmap0 empty\n");
			} else {
		        str = hashmap0_next(h, &iter);
				if (str) {
	    	    	tcase_printf(verbose, "get %s %d\n", str, hashmap0_size(h));
					if ((str = hashmap0_remove(h, str)) == NULL) {
						errno = EFAULT;
						PMNO(errno);
						return -1;
					}
/*fputc('-', stderr);
*/
					tcase_printf(verbose, "remove %s %d\n", str, hashmap0_size(h));
					allocator_free(NULL, str);
				} else {
/*fputc('x', stderr);
*/
					tcase_printf(verbose, "nothing left to iterate over\r");
					hashmap0_iterate(h, &iter);
				}
			}
		}
    }
fprintf(stderr, "%2d %8ld %8d %8d %8d\n", chkpnt++, (clock() - t0) / 1000, hashmap0_size(h), HASHMAP_LARGE, h->al->size_total);

	hashmap0_del(h);
	tcase_printf(verbose, "hashmap0 done\n");

	cfg = NULL;
    return 0;
}
