#include "common.h"
#include <time.h>
#include "mba/hashmap.h"

int
HashmapCheck(int verbose, struct cfg *cfg, char *args[])
{
	struct hashmap *h;
	struct linkedlist *l;
    int rate = 0, i, n = 0;
	int count = atoi(args[0]);
	int interval = atoi(args[1]);
	iter_t riter;
	char *str;
	clock_t t0;
	int chkpnt = 0;

	if ((h = hashmap_new(hash_str, cmp_str, NULL, NULL)) == NULL ||
			(l = linkedlist_new(0, NULL)) == NULL) {
		AMSG("");
		return -1;
	}

	t0 = clock();
if (verbose)
fprintf(stderr, "\n       time    count     size     rate\n");

	rate_iterate(&riter);
	for (i = 0; i < count; i++) {
		if ((i % interval) == 0) {
			rate = rate_next(&riter);
if (verbose)
fprintf(stderr, "%2d %8ld %8d %8d %8d\n", chkpnt++, (clock() - t0) / 1000, hashmap_size(h), table_sizes[h->table_size_index], rate);
		}

		if (rand() % 10 < rate) {
	        str = allocator_alloc(NULL, 8, 0);
	        sprintf(str, "%07d", n++);

	        if (hashmap_put(h, str, str) == -1 ||
					linkedlist_add(l, str) == -1) {
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
				int siz = linkedlist_size(l) - 1;
				int ran = randint(0, siz);

				str = linkedlist_remove(l, ran);

				if (str) {
					char *data;
					char *key = str;

	    	    	tcase_printf(verbose, "rem %s %d\r", str, hashmap_size(h));
					if (hashmap_remove(h, (void **)&key, (void **)&data) == -1) {
						AMSG("Failed to remove element: %s", str);
						return -1;
					}
/*fputc('-', stderr);
*/
					allocator_free(NULL, key);
				} else {
					MMSG("No such element: %d", ran);
				}
			}
		}
    }

if (verbose)
fprintf(stderr, "%2d %8ld %8d %8d\n", chkpnt++, (clock() - t0) / 1000, hashmap_size(h), table_sizes[h->table_size_index]);
	hashmap_del(h, NULL, NULL, NULL);
	linkedlist_del(l, allocator_free, NULL);

	cfg = NULL;
    return 0;
}
