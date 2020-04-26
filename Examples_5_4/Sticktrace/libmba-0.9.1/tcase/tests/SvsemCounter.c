#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "common.h"
#include "mba/cfg.h"
#include "mba/svsem.h"

/* args[0] - number of processes
 * args[1] - increments per process
 *
 * With 20 * 1000000 hexdump should report:
 * $ hexdump /tmp/SvsemCounter.shm 
 * 0000000 2d00 0131
 * With 20 * 100000 hexdump should report:
 * $ hexdump /tmp/SvsemCounter.shm 
 * 0000000 8480 001e
 */

static int count = 0;

int
run(void)
{
	int fd, *ptr, i;
	svsem_t sem;

	if ((fd = open("/tmp/SvsemCounter.shm", O_RDWR | O_CREAT, 0600)) == -1 ||
			ftruncate(fd, sizeof *ptr) == -1 ||
			(ptr = (int *)mmap(NULL, sizeof *ptr, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == NULL) {
		return -1;
	}
	close(fd);

	if (svsem_open(&sem, "/tmp/SvsemCounter.sem", SEM_UNDO) == -1) {
		return -1;
	}

	for (i = 0; i < count; i++) {
		svsem_wait(&sem);
		(*ptr)++;
		svsem_post(&sem);
	}

	svsem_close(&sem);
	munmap((void *)ptr, sizeof *ptr);

	return 0;
}
int
SvsemCounter(int verbose, struct cfg *cfg, char *args[])
{
	svsem_t sem;
	int pi, nprocs, *pids, fd, *ptr, status;
	const char *path = "/tmp/SvsemCounter.sem";

	if (svsem_open(&sem, path, O_CREAT | SEM_UNDO, 0600, 1) == -1) {
		if (errno != EEXIST) {
			perror("svsem_open");
			return EXIT_FAILURE;
		}
	}

	if ((fd = open("/tmp/SvsemCounter.shm", O_RDWR | O_CREAT, 0600)) == -1 ||
			ftruncate(fd, sizeof *ptr) == -1 ||
			(ptr = (int *)mmap(NULL, sizeof *ptr, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == NULL) {
		perror("");
		return -1;
	}
	close(fd);

	nprocs = strtoul(args[0], NULL, 10);
	count = strtoul(args[1], NULL, 10);

	if ((pids = malloc(nprocs * sizeof *pids)) == NULL) {
		perror("");
		return -1;
	}

	*ptr = 0;
	for (pi = 0; pi < nprocs; pi++) {
		pid_t pid;
		if ((pid = fork()) == -1) {
			perror("");
			return EXIT_FAILURE;
		}
		if (pid) {
			pids[pi] = pid;
		} else {
			int ret;
			ret = run();
			free(pids);
			munmap(ptr, sizeof *ptr);
			svsem_close(&sem);
			exit(ret);
		}
	}
	do {
		waitpid(pids[--pi], &status, 0);
		if ((errno = WEXITSTATUS(status))) {
			perror("");
		}
		tcase_printf(verbose, "process complete\n");
	} while (pi);

	if (*ptr != (nprocs * count)) {
		tcase_printf(verbose, "nprocs=%d,count=%d,ptr=%d\n", nprocs, count, *ptr);
		return -1;
	}

	free(pids);
	munmap(ptr, sizeof *ptr);
	if (svsem_close(&sem) != 0) {
		MMSG("");
	}

	tcase_printf(verbose, "done");

	cfg = NULL;
	return 0;
}
