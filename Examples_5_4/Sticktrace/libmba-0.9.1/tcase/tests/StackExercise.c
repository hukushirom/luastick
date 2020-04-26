#include "common.h"
#include "mba/stack.h"

int
StackExercise(int verbose, struct cfg *cfg, char *args[])
{
    int rate, i, n = 0;
	char *str;
    struct stack *s = stack_new(EXERCISE_LRG_COUNT, NULL);
	cfg = NULL; args[0] = NULL;

	rate = EXERCISE_R0;
	for (i = 0; i < EXERCISE_LRG_COUNT; i++) {
		if (i == EXERCISE_LRG_P1) {
			rate = EXERCISE_R1;
		} else if (i == EXERCISE_LRG_P2) {
			rate = EXERCISE_R2;
		} else if (i == EXERCISE_LRG_P3) {
			rate = EXERCISE_R3;
		}

		if (rand() % 10 < rate) {
	        str = malloc(8);
	        sprintf(str, "%07d", n++);
	        if (stack_push(s, str) == -1) {
				PMSG("stack full");
				return -1;
			} else {
	 	      	tcase_printf(verbose, "PUSH  %s %d\n", str, stack_size(s));
			}
		} else {
			if (stack_is_empty(s)) {
				tcase_printf(verbose, "stack empty\n");
			} else {
		        str = stack_pop(s);
				if (str) {
	    	    	tcase_printf(verbose, "POPPED %s %d\n", str, stack_size(s));
			       	free(str);
				} else {
					PMSG("error popping from non-empty stack\n");
					return -1;
				}
			}
		}
    }

	stack_del(s, allocator_free, NULL);

    return 0;
}
