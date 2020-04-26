#include "common.h"
#include "mba/eval.h"

int
EvalExamples(int verbose, struct cfg *cfg, char *args[])
{
	unsigned char buf[1024], *row[10];
	FILE *in;
	int ret;
	struct eval *eval;
	unsigned long val, result;
	cfg = NULL;

	if ((in = fopen(args[0], "r")) == NULL) {
		PMNO(errno);
		return -1;
	}

	if ((eval = eval_new(NULL, NULL)) == NULL) {
		ret = -1;
		AMSG("");
		fclose(in);
		return -1;
	}

	while ((ret = csv_row_fread(in, buf, 1024, row, 10, ',', CSV_QUOTES | CSV_TRIM)) > 0) {

		if (eval_expression(eval, row[0], row[0] + strlen(row[0]), &result) == -1) {
			ret = -1;
			AMSG("");
			goto out;
		}

		val = strtoul(row[1], NULL, 0);
		if (result != val) {
			ret = -1;
			PMSG("Unexpected result: %lu != %lu", result, val);
			goto out;
		}

		tcase_printf(verbose, "%s = %ld\n", row[0], result);
	}

out:
	eval_del(eval);
	fclose(in);

    return ret;
}
