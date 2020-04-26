#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <locale.h>
#include <limits.h>
#include <mba/cfg.h>
#include <mba/csv.h>
#include <mba/msgno.h>

typedef int (*test_fn)(int verbose, struct cfg *cfg, char *args[]);

int PoolExercise(int verbose, struct cfg *cfg, char *args[]);
int PathCanonExamples(int verbose, struct cfg *cfg, char *args[]);
int HashmapExercise(int verbose, struct cfg *cfg, char *args[]);
int HashmapCount(int verbose, struct cfg *cfg, char *args[]);
int HashmapCheck(int verbose, struct cfg *cfg, char *args[]);
int LinkedlistExercise(int verbose, struct cfg *cfg, char *args[]);
int CsvExamples(int verbose, struct cfg *cfg, char *args[]);
int CsvEol(int verbose, struct cfg *cfg, char *args[]);
int StackExercise(int verbose, struct cfg *cfg, char *args[]);
int VarrayOps(int verbose, struct cfg *cfg, char *args[]);
int CfgOps(int verbose, struct cfg *cfg, char *args[]);
int CfgQueryStringExamples(int verbose, struct cfg *cfg, char *args[]);
int CfgSuba(int verbose, struct cfg *cfg, char *args[]);
int TcharAll(int verbose, struct cfg *cfg, char *args[]);
int TextTest(int verbose, struct cfg *cfg, char *args[]);
int DomnodeLoadStore(int verbose, struct cfg *cfg, char *args[]);
int DomnodeOps(int verbose, struct cfg *cfg, char *args[]);
#if !defined(_WIN32) && !defined(_HPUX_SOURCE)
int ShelloutSimple(int verbose, struct cfg *cfg, char *args[]);
#endif
int EvalExamples(int verbose, struct cfg *cfg, char *args[]);
int SvsemCounter(int verbose, struct cfg *cfg, char *args[]);
int SubaExercise(int verbose, struct cfg *cfg, char *args[]);
int AdtInit(int verbose, struct cfg *cfg, char *args[]);
int SharedAdts(int verbose, struct cfg *cfg, char *args[]);
int BitsetOps(int verbose, struct cfg *cfg, char *args[]);
int SvcondProdCons(int verbose, struct cfg *cfg, char *args[]);
int DiffExamples(int verbose, struct cfg *cfg, char *args[]);
int OpenCre(int verbose, struct cfg *cfg, char *args[]);

struct test {
	int num;
	test_fn test;
} test_tbl[] = {
	{ 100, PoolExercise },
	{ 200, PathCanonExamples },
	{ 300, HashmapExercise },
	{ 302, HashmapCheck },
	{ 400, LinkedlistExercise },
	{ 500, CsvExamples },
	{ 501, CsvExamples },
	{ 502, CsvExamples },
	{ 503, CsvExamples },
	{ 510, CsvEol },
	{ 511, CsvEol },
	{ 512, CsvEol },
	{ 600, StackExercise },
	{ 700, VarrayOps },
	{ 800, CfgOps },
	{ 802, CfgSuba },
	{ 900, TcharAll },
	{ 901, TextTest },
#if !defined(_WIN32)
	{ 301, HashmapCount },
	{ 801, CfgQueryStringExamples },
#if !defined(_HPUX_SOURCE)
	{ 1000, ShelloutSimple },
#endif
	{ 5100, EvalExamples },
	{ 5400, AdtInit },
	{ 5500, SharedAdts },
	{ 5700, SvcondProdCons },
	{ 5200, SvsemCounter },
	{ 5300, SubaExercise },
	{ 5301, SubaExercise },
	{ 5302, SubaExercise },
	{ 5303, SubaExercise },
	{ 5304, SubaExercise },
	{ 5305, SubaExercise },
	{ 5306, SubaExercise },
	{ 5307, SubaExercise },
	{ 5308, SubaExercise },
	{ 5309, SubaExercise },
	{ 5310, SubaExercise },
	{ 5311, SubaExercise },
	{ 5312, SubaExercise },
	{ 5313, SubaExercise },
	{ 5314, SubaExercise },
	{ 5315, SubaExercise },
	{ 5316, SubaExercise },
	{ 5317, SubaExercise },
	{ 5318, SubaExercise },
	{ 5319, SubaExercise },
	{ 5320, SubaExercise },
#endif
/* DEPRECATED
	{ 1200, MbsSimple },
	{ 1201, MbsSimple },
	{ 1202, MbsSimple },
	{ 1203, MbsSimple },
	{ 1204, MbsSimple },
	{ 1205, MbsSimple },
	{ 1206, MbsSimple },
	{ 1207, MbsSimple },
	{ 1208, MbsSimple },
	{ 1209, MbsSimple },
	{ 1210, MbsSimple },
	{ 1211, MbsSimple },
	{ 1212, MbsSimple },
	{ 1213, MbsSimple },
	{ 1214, MbsSimple },
	{ 1215, MbsSimple },
	{ 1216, MbsSimple },
	{ 1217, MbsSimple },
	{ 1218, MbsSimple },
	{ 5000, DomnodeLoadStore },
	{ 5001, DomnodeOps },
*/
	{ 5600, BitsetOps },
	{ 5800, DiffExamples },
	{ 5900, OpenCre },
	{ 0, NULL }
};

int
run_suite(const char *cfgname, const char *csvname, int testnums[], int tlen)
{
	struct cfg *cfg;
	FILE *csv;
	unsigned char buf[4096], *row[32];
	int tnum, ti, ret, flags;

	if ((cfg = cfg_new(NULL)) == NULL ||
				cfg_load(cfg, cfgname)) {
		AMSG(": %s", cfgname);
		return -1;
	}
	if ((csv = fopen(csvname, "r")) == NULL) {
		PMNF(errno, ": %s", csvname);
		return -1;
	}

	while ((ret = csv_row_fread(csv, buf, 4096, row, 32, ',', CSV_TRIM | CSV_QUOTES)) > 0) { 
		if ((flags = strtol(row[1], NULL, 10)) == LONG_MIN || flags == LONG_MAX) {
			PMNF(errno, ": Invalid test flags: %s", row[1]);
			return -1;
		}
		if (flags == 0) {
			continue;
		}
		if ((tnum = strtol(row[0], NULL, 10)) == LONG_MIN || tnum == LONG_MAX) {
			PMNF(errno, ": Invalid test number: %s", row[0]);
			return -1;
		}
		if (tlen) {
			for (ti = 0; ti < tlen; ti++) {
				if (tnum == testnums[ti]) {
					break;
				}
			}
			if (ti == tlen) {
				continue;
			}
		}

		for (ti = 0; test_tbl[ti].test; ti++) {
			if (tnum == test_tbl[ti].num) {
				break;
			}
		}
		if (test_tbl[ti].test == NULL) {
			PMSG("Test not found: %d", tnum);
			return -1;
		}

		errno = 0;

		fprintf(stderr, "%d:%s: ", tnum, row[2]);
		fflush(stderr);
		if (test_tbl[ti].test(flags > 1 ? flags : 0, cfg, (char **)row + 4) == 0) {
			fprintf(stderr, "pass\n");
		} else {
			MMSG("%d failed", tnum);
		}
	}

	fclose(csv);
	cfg_del(cfg);

	return ret;
}

int
main(int argc, char *argv[])
{
	char *arg, *cfgname, *csvname;
	int ai, testnums[256], ti, num;

	setlocale(LC_CTYPE, "");

	cfgname = csvname = NULL;
	for (ai = 1; ai < argc; ai++) {
		arg = argv[ai];
		if (strcmp("-f", arg) == 0) {
			arg = argv[++ai];
			if (ai >= argc) {
				PMNF(EINVAL, ": -f requires a .cfg filename");
				goto err;
			}
			cfgname = arg;
		} else if (strcmp("-t", arg) == 0) {
			arg = argv[++ai];
			if (ai >= argc) {
				PMNF(EINVAL, ": -t requires a .csv filename");
				goto err;
			}
			csvname = arg;
		} else {
			break;
		}
	}
	if (cfgname == NULL) {
		cfgname = "tmba.cfg";
	}
	if (csvname == NULL) {
		csvname = "tmba.csv";
	}

	ti = 0;
	while (ai < argc) {
		arg = argv[ai++];
		if ((num = strtol(arg, NULL, 10)) == LONG_MIN || num == LONG_MAX) {
			PMNF(errno, ": Invalid test number: %s", arg);
			goto err;
		}
		testnums[ti++] = num;
	}

	if (run_suite(cfgname, csvname, testnums, ti) == -1) {
		AMSG("");
		goto err;
	}

	return EXIT_SUCCESS;
err:
	MMSG("usage: %s [-f <suite.cfg>] [-t <suite.csv>] [<test num> <test num> <test num> ...]\n", argv[0]);
	return EXIT_FAILURE;
}

