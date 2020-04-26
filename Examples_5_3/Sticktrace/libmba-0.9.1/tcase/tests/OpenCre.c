#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include "common.h"
#include "mba/cfg.h"
#include "mba/time.h"
#include "mba/svsem.h"
#include "mba/misc.h"

static void
run(svsem_t *sem, int n, int verbose)
{
	int i, rst;
	char buf[255];

	for (i = 0; i < n; i++) {
		sprintf(buf, "/tmp/file.%d", i);
		svsem_wait(sem);

		if (copen(buf, O_CREAT, 0600, &rst) == -1) {
			MMSG("");
		} else if (verbose) {
printf("%d %d: %d %s\n", getpid(), i, rst, buf);
		}

		usleep(100);
	}
	
}

int
OpenCre(int verbose, struct cfg *cfg, char *args[])
{
	int nprocs = atoi(args[0]), n = atoi(args[1]), i, j, status;
	pid_t *pids;
	svsem_t sem;

	if (svsem_create(&sem, 0, 1) == -1) {
		AMSG("");
		return -1;
	}

	if ((pids = malloc(nprocs * sizeof *pids)) == NULL) {
		PMNO(errno);
		return -1;
	}

	for (i = 0; i < nprocs; i++) {
		if ((pids[i] = fork()) == 0) {
			run(&sem, n, verbose);
			exit(0);
		}
	}
	for (j = 0; j < n; j++) {
		usleep(200);
		if (verbose) {
			printf("%d\n", j);
		}
		svsem_post_multiple(&sem, nprocs);
	}
	do {
		waitpid(pids[--i], &status, 0);
		if ((errno = WEXITSTATUS(status))) {
			perror("");
		}
		tcase_printf(verbose, "process complete\n");
	} while(i);

	free(pids);
	svsem_destroy(&sem);

	tcase_printf(verbose, "done");
	cfg = NULL;
    return 0;
}
