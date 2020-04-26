#include "common.h"

int
CfgQueryStringExamples(int verbose, struct cfg *cfg, char *args[])
{
	unsigned char buf[1024], *row[10];
	FILE *in;
	int ret;

	if ((in = fopen(args[0], "r")) == NULL) {
		PMNO(errno);
		return -1;
	}

	while ((ret = csv_row_fread(in, buf, 1024, row, 10, ',', CSV_QUOTES | CSV_TRIM)) > 0) {
    	int success = atoi(row[0]);

		tcase_printf(verbose, "%s:\n", row[1]);

		cfg_clear(cfg);

		if (cfg_load_cgi_query_string(cfg, row[1], row[1] + tcslen(row[1])) == -1) {
			if (success) {
				ret = -1;
				AMSG("");
				goto out;
			}
		} else if (!success) {
			ret = -1;
			AMSG("Supposed to fail");
			goto out;
		}

		if (verbose) {
			iter_t iter;
			const tchar *name;
			tchar dst[512];

			cfg_iterate(cfg, &iter);
			while ((name = cfg_next(cfg, &iter))) {
				if (cfg_get_str(cfg, dst, 512, NULL, name) == -1) {
					errno = ENOENT;
					PMNO(errno);
					return -1;
				}
				tcase_printf(verbose, "\t%s=%s\n", name, dst);
			}
		}
	}

out:
	fclose(in);

    return ret;
}
