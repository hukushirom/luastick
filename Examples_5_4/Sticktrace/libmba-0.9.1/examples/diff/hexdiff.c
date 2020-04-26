#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <mba/diff.h>
#include <mba/hexdump.h>
#include <mba/msgno.h>

#define HEXWIDTH 32

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
	unsigned char *a, *b;
	int n, m, d;
	int sn, i;
	struct varray *ses = varray_new(sizeof(struct diff_edit), NULL);

	if (argc < 3) {
		fprintf(stderr, "usage: %s <file1> <file2>\n", argv[0]);
		return EXIT_FAILURE;
	}

	if ((a = mapfile(argv[1], &n)) == NULL || (b = mapfile(argv[2], &m)) == NULL) {
		MMSG("");
		return EXIT_FAILURE;
	}

	if ((d = diff(a, 0, n, b, 0, m, NULL, NULL, NULL, 0, ses, &sn, NULL)) == -1) {
		MMNO(errno);
		return EXIT_FAILURE;
	}

	for (i = 0; i < sn; i++) {
		struct diff_edit *e = varray_get(ses, i);

		switch (e->op) {
			case DIFF_MATCH:
				printf("MAT:\n");
				hexdump(stdout, a + e->off, e->len, HEXWIDTH);
				break;
			case DIFF_INSERT:
				printf("INS:\n");
				hexdump(stdout, b + e->off, e->len, HEXWIDTH);
				break;
			case DIFF_DELETE:
				printf("DEL:\n");
				hexdump(stdout, a + e->off, e->len, HEXWIDTH);
				break;
		}
	}
	printf("edit distance: %d\nscript length: %d\n", d, sn);

	return EXIT_SUCCESS;
}

