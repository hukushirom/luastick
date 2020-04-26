#include "common.h"
#include "mba/csv.h"

#include "mba/hexdump.h"

int
readline(FILE *in, char *src)
{
	char *start = src;
	int ch;

	while ((ch = fgetc(in)) != EOF) {
		if (ch == '\n' || ch == '\0') {
			*src = '\0';
			return src - start;
		}
		*src++ = ch;
	}

	return 0;
}

int
CsvExamples(int verbose, struct cfg *cfg, char *args[])
{
	unsigned char buf[4096], inbuf[4096], exbuf[4096], *row[100];
	FILE *in, *ex;
	int ret, i, j, sep, flags, count = 1;
	cfg = NULL;

	if ((in = fopen(args[0], "r")) == NULL) {
		PMNF(errno, ": %s", args[0]);
		return -1;
	}
	if ((ex = fopen(args[1], "r")) == NULL) {
		PMNF(errno, ": %s", args[1]);
		return -1;
	}

	flags = atoi(args[2]);
	sep = *args[3];
	while ((ret = csv_row_fread(in, buf, 4096, row, 100, sep, flags)) > 0) {
		if (readline(ex, exbuf) < 1) {
			break;
		}
		j = 0;
		for (i = 0; i < 100; i++) {
			if (row[i]) {
				inbuf[j++] = '{';
				strcpy(inbuf + j, row[i]);
				j += strlen(row[i]);
				inbuf[j++] = '}';
			}
		}
		inbuf[j++] = '\0';

		if (strcmp(inbuf, exbuf) != 0) {
			PMSG("Failed to match %d:\nin[%s]\nex[%s]", count, inbuf, exbuf);
/*
hexdump(stderr, inbuf, 32, 16);
hexdump(stderr, exbuf, 32, 16);
*/
			return -1;
		}

		tcase_printf(verbose, "%40s %40s\n", inbuf, exbuf);
		count++;
	}

	fclose(ex);
	fclose(in);

    return ret;
}
