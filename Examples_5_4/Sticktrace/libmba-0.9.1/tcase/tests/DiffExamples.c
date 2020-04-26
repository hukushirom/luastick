#include "common.h"
#include "mba/diff.h"

int
DiffExamples(int verbose, struct cfg *cfg, char *args[])
{
	unsigned char buf[1024], *row[10], *a, *b;
	FILE *in;
	int ret, t = 1;
	struct varray *ses = varray_new(sizeof(struct diff_edit), NULL);
	struct varray *ibuf = varray_new(sizeof(int), NULL);

	if ((in = fopen(args[0], "r")) == NULL) {
		PMNO(errno);
		return -1;
	}

	while ((ret = csv_row_fread(in, buf, 1024, row, 10, ',', CSV_QUOTES | CSV_TRIM)) > 0) {
		int n, m, d, sn, i;
		unsigned char tmp[1024], *p;
		struct diff_edit *e;

		a = row[2];
		b = row[3];
		n = strlen(a);
		m = strlen(b);

		if ((d = diff(a, 0, n, b, 0, m, NULL, NULL, NULL, 0, ses, &sn, ibuf)) == -1) {
			AMSG("");
			return -1;
		}
		if (d != atoi(row[0])) {
			PMSG("%d: edit distance incorrect: expected=%d result=%d: %s %s\n", t, atoi(row[0]), d, a, b);
			return -1;
		} else if (sn != atoi(row[1])) {
			PMSG("%d: script length incorrect: expected=%d result=%d: %s %s\n", t, atoi(row[1]), sn, a, b);
			return -1;
		}

		p = tmp;

		for (i = 0; i < sn; i++) {
			e = varray_get(ses, i);
			if (e->op == DIFF_MATCH || e->op == DIFF_INSERT) {
				memcpy(p, (e->op == DIFF_MATCH ? a : b) + e->off, e->len);
				p += e->len;
			}
		}

		if ((p - tmp) != m || memcmp(b, tmp, m)) {
			PMSG("%d: invalid script: %d != %d: %s ->", t, (p - tmp), m, b);
			fwrite(tmp, 1, m, stderr);
			fputc('\n', stderr);
			return -1;
		}

		tcase_printf(verbose, "%d: %d %d %-15s --> %-15s\n", t++, d, sn, a, b);
	}

	fclose(in);

	cfg = NULL;
    return ret;
}
