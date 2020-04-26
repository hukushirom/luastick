#include "common.h"
#include "mba/csv.h"

int
CsvEol(int verbose, struct cfg *cfg, char *args[])
{
	const unsigned char *input = args[0];
	size_t n = strlen(input);
	unsigned char buf[1024];
	unsigned char *row[10];

	if (csv_row_parse_str(input, n + 10, buf, 1024, row, 10, ',', CSV_TRIM | CSV_QUOTES) == -1) {
		AMSG("%s", input);
		return -1;
	}

	tcase_printf(verbose, "%s|%s|%s\n", row[0], row[1], row[2]);
	
	cfg = NULL;
    return 0;
}
