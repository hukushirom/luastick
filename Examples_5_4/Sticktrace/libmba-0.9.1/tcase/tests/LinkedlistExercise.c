#include "common.h"
#include "mba/linkedlist.h"

int
LinkedlistExercise(int verbose, struct cfg *cfg, char *args[])
{
    int rate, i, n = 0, idx;
	char *str;
    struct linkedlist *l = linkedlist_new(EXERCISE_MED_COUNT, NULL);

	cfg = NULL; args[0] = NULL;

	if (l == NULL) {
		AMSG("");
		return -1;
	}

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
			idx = 0;
	        str = malloc(8);
	        sprintf(str, "%07d", n++);
			if (rand() % 5) {
				idx = linkedlist_size(l);
				if (idx) {
					idx = rand() % idx;
				}
			}
	        if (linkedlist_insert(l, idx, str) == -1) {
				PMNO(errno);
				return -1;
			}
			tcase_printf(verbose, "INSERT: %s size now %d\n", str, linkedlist_size(l));
		} else {
			if (linkedlist_is_empty(l)) {
				tcase_printf(verbose, "EMPTY\n");
			} else {
				idx = rand() % linkedlist_size(l);
		        str = linkedlist_get(l, idx);
				if (linkedlist_remove_data(l, str) == NULL) {
					PMNO(errno);
					return -1;
				}
				if ((idx % 10) == 0) {
					unsigned int count = 0;
					iter_t iter;

					linkedlist_iterate(l, &iter);
					while (linkedlist_next(l, &iter)) {
						count++;
					}
					if (count != linkedlist_size(l)) {
						PMSG("count=%u,linkedlist_size=%u\n", count, linkedlist_size(l));
						return -1;
					}
				}
				if (str) {
	    	    	tcase_printf(verbose, "REMOVE: %s %d\n", str, linkedlist_size(l));
			       	free(str);
				} else {
					PMSG("remove failure");
					return -1;
				}
			}
		}
    }
    linkedlist_del(l, allocator_free, NULL);

    return 0;
}

