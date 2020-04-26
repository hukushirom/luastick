#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <mba/msgno.h>
#include <mba/csv.h>

/* generate text.h prototypes from text.csv
 * (only used once)
 */

#define DO_STRING   1
#define DO_WCHAR    2
#define DO_PROTOS   4

int
run(FILE *input, int flags)
{
	char buf[4096];
	char *row[9];

	/* 0 char    ret
	 * 1 char    name
	 * 2 char    args
	 * 3 wchar_t ret
	 * 4 wchar_t name
	 * 5 wchar_t args
	 * 6 tchar   ret
	 * 7 tchar   name
	 * 8 tchar   args
	 */
	while (csv_row_fread(input, buf, 1024, row, 10, 1)) {
		if (row[7] == NULL || *row[7] == '\0') {
			continue;
		}
		if ((flags & DO_PROTOS)) {
			char *str = row[6];
			char *fmt = str[strlen(str) - 1] == '*' ? "%s%s%s\n" : "%s %s%s\n";
			if ((flags & DO_STRING)) {
				printf(fmt, row[0], row[1], row[2]);
			} else if ((flags & DO_WCHAR)) {
				printf(fmt, row[3], row[4], row[5]);
			} else {
				printf(fmt, row[6], row[7], row[8]);
			}
		} else {
			int which = (flags & DO_WCHAR) ? 4 : 1;
			printf("#define %s %s\n", row[7], row[which]);
		}
	}

	return 0;
}
int
main(int argc, char *argv[])
{
	FILE *input;
	int flags = 0;
	int argi;
	char *arg, *filename = NULL;

	errno = 0;

	argi = 1;
	while (argi < argc) {
		arg = argv[argi];
		if (strcmp(arg, "-s") == 0) {
			flags |= DO_STRING;
		} else if (strcmp(arg, "-w") == 0) {
			flags |= DO_WCHAR;
		} else if (strcmp(arg, "-p") == 0) {
			flags |= DO_PROTOS;
		} else {
			filename = arg;
		}
		argi++;
	}

	if (filename == NULL) {
		goto err;
	}

	if ((input = fopen(filename, "r")) == NULL || run(input, flags) == -1) {
		MNF(errno, ": %s", argv[1]);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
err:
	fprintf(stderr, "usage: %s [-w] <csvfile>\n", argv[0]);
	return EXIT_FAILURE;
}
