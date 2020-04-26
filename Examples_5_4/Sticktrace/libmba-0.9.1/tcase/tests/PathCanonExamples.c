#include "common.h"
#include "mba/path.h"

int
PathCanonExamples(int verbose, struct cfg *cfg, char *args[])
{
	unsigned char buf[1024], *input, output[1024], *row[10];
	FILE *in;
	int ret;
	cfg = NULL;

	if ((in = fopen(args[0], "r")) == NULL) {
		PMNO(errno);
		return -1;
	}

	while ((ret = csv_row_fread(in, buf, 1024, row, 10, ',', CSV_QUOTES | CSV_TRIM)) > 0) {
		input = row[0];
		if (path_canon(input, input + strlen(input) + 1, output, output + 1024, '/', '/') == -1) {
			AMSG("");
			return -1;
		}
		if (strcmp(output, row[1]) != 0) {
			printf("Failure:\n   input: %s\nexpected: %s\n  output: %s\n", input, row[1], output);
			return -1;
		}
		tcase_printf(verbose, "%-15s --> %-15s\n", input, output);
	}

	fclose(in);

    return ret;
}
