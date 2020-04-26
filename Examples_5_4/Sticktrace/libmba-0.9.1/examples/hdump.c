#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <mba/hexdump.h>
#include <mba/msgno.h>

#define DEFAULT_WIDTH 16

void *
mapfile(const char *filename, int *size)
{
	void *ret = NULL;
	int fd;
	struct stat st;

	if ((fd = open(filename, O_RDONLY)) == -1 || fstat(fd, &st) == -1) {
		PMNO(errno);
		return NULL;
	}
	if ((ret = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
		PMNO(errno);
		return NULL;
	}

	*size = st.st_size;

	return ret;
}
int
main(int argc, char *argv[])
{
	unsigned char *a, *arg;
	int n, width = DEFAULT_WIDTH;

	if (argc < 2) {
		fprintf(stderr, "usage: %s [-<width>] <file>\n", argv[0]);
		return EXIT_FAILURE;
	}

	arg = argv[1];

	if (*arg == '-') {
		arg++;
		width = atoi(arg);
		arg = argv[2];
	}

	if ((a = mapfile(arg, &n)) == NULL) {
		MSG("");
		return EXIT_FAILURE;
	}

	hexdump(stdout, a, n, width);

	return EXIT_SUCCESS;
}

