#include "common.h"
#include <time.h>
#include "mba/hashmap1.h"
#include "mba/suba.h"

int
HashmapExercise1(int verbose, struct cfg *cfg, char *args[])
{
	struct hashmap1 *h;
    int rate = 0, i, n = 0;
	int count = atoi(args[0]);
	int interval = atoi(args[1]);
	iter_t riter;
	iter_t iter;
	char *str;
	struct allocator *al; 
	char *mem;
	clock_t t0;
	int chkpnt = 0;

	if ((mem = malloc(0xFFFFFFF)) == NULL ||
			(al = suba_init(mem, 0xFFFFFFF, 1, 0)) == NULL ||
			(h = hashmap1_new(hash_str, (compare_fn)strcmp, al)) == NULL) {
		AMSG("");
		return -1;
	}
srand(1);

fputc('\n', stderr);

	t0 = clock();
fprintf(stderr, "       time    count     size      mem\n");

	hashmap1_iterate(h, &iter);
	rate_iterate(&riter);
	for (i = 0; i < count; i++) {
		if ((i % interval) == 0) {
			int level = 0;
			while (h->tables[level + 1]) {
				level++;
			}
fprintf(stderr, "%2d %8ld %8d %8d %8d %d\n", chkpnt++, (clock() - t0) / 1000, hashmap1_size(h), total_sizes[level], h->al->size_total - h->al->free_total, rate);
			rate = rate_next(&riter);
		}

		if (rand() % 10 < rate) {
	        str = allocator_alloc(NULL, 16, 0);
	        sprintf(str, "%015d", n++);
	        if (hashmap1_put(h, str, str) == -1) {
				AMSG("");
				return -1;
			} else {
/*fputc('+', stderr);
*/
	 	      	tcase_printf(verbose, "put %s %d\r", str, hashmap1_size(h));
			}
		} else {
			if (hashmap1_is_empty(h)) {
/*fputc('0', stderr);
*/
				tcase_printf(verbose, "hashmap1 empty\r");
			} else {
		        str = hashmap1_next(h, &iter);
				if (str) {
	    	    	tcase_printf(verbose, "get %s %d\r", str, hashmap1_size(h));
					if ((str = hashmap1_remove(h, str)) == NULL) {
						errno = EFAULT;
						PMNO(errno);
						return -1;
					}
/*fputc('-', stderr);
*/
					tcase_printf(verbose, "rem %s %d\r", str, hashmap1_size(h));
					allocator_free(NULL, str);
				} else {
/*fputc('x', stderr);
*/
					tcase_printf(verbose, "nothing left to iterate over\r");
					hashmap1_iterate(h, &iter);
				}
			}
		}
    }
{
	int level = 0;
	while (h->tables[level + 1]) {
		level++;
	}
fprintf(stderr, "%2d %8ld %8d %8d %8d\n", chkpnt++, (clock() - t0) / 1000, hashmap1_size(h), total_sizes[level], h->al->size_total);
}

	hashmap1_del(h, allocator_free, NULL, al);
	tcase_printf(verbose, "\nhashmap1 done\n");

	cfg = NULL;
    return 0;
}
