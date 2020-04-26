#include "common.h"

int
BlankExercise(int verbose, struct cfg *cfg, char *args[])
{
    int rate, i;
	cfg = NULL; args[0] = NULL;

	rate = EXERCISE_R0;
	for (i = 0; i < EXERCISE_SM_COUNT; i++) {
		if (i == EXERCISE_SM_P1) {
			rate = EXERCISE_R1;
		} else if (i == EXERCISE_SM_P2) {
			rate = EXERCISE_R2;
		} else if (i == EXERCISE_SM_P3) {
			rate = EXERCISE_R3;
		}

		if (rand() % 10 < rate) {
		} else {
		}
    }

	tcase_printf(verbose, "done\n");

    return 0;
}
