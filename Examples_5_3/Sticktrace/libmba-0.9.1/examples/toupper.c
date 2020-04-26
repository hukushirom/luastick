#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <locale.h>

#include <mba/msgno.h>
#include <mba/text.h>

#define BSIZE 0xFFFF

int
main(int argc, char *argv[])
{
	FILE *fp;
	size_t n;
	unsigned char buf[BSIZE];
	int lower = 0;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <utf8file> [0]\n", argv[0]);
		return EXIT_FAILURE;
	}

	errno = 0;

	if ((fp = fopen(argv[1], "r")) == NULL) {
		MMNF(errno, ": %s", argv[1]);
		return EXIT_FAILURE;
	}

	if (argc > 2) {
		lower = strcmp("0", argv[2]) == 0;
	}

	if (!setlocale(LC_CTYPE, "")) {
		MMSG("failed to setlocale");
		return EXIT_FAILURE;
	}

	while ((n = fread(buf, 1, BSIZE, fp)) > 0) {
		if (lower) {
			if (utf8tolower(buf, buf + n) != (int)n) {
				MMSG("conversion failure");
				return EXIT_FAILURE;
			}
		} else {
			if (utf8toupper(buf, buf + n) != (int)n) {
				MMSG("conversion failure");
				return EXIT_FAILURE;
			}
		}
		if (fwrite(buf, 1, n, stdout) != n) {
			MMSG("write failure");
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
