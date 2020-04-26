#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <ctype.h>
#include "mba/hexdump.h"

#include "common.h"
#include "mba/cfg.h"
#include "mba/mbs.h"

struct record {
	char *str;
	size_t len;
	int nchars;
};

static int
run(int verbose, struct record *rec)
{
	char *s;
	char *o;
	int n;
	wchar_t wc;

	if (verbose) {
		hexdump(stdout, rec->str, rec->len, 16);
	}

	if ((s = mbsndup(rec->str, rec->len, -1)) == NULL) {
		AMSG("Failure");
		return -1;
	}

	tcase_printf(verbose, "nchars=%u,len=%u,str=[%s]\n", rec->nchars, rec->len, s);

	if (strncmp(s, rec->str, rec->len)) {
		PMSG("strcmp failed: [%s]!=[%s]", s, rec->str);
		return -1;
	}
	if (mbssize(s) != rec->len) {
		PMSG("mbssize=%u,len=%u", mbssize(s), rec->len);
		return -1;
	}
	if ((o = mbsnoff(rec->str, rec->nchars, -1)) == NULL) {
		AMSG("");
		return -1;
	}
	if ((size_t)(o - rec->str) < rec->len) {
		PMSG("o - str=%u,len=%u,o=[%s]", (o - rec->str), rec->len, mbsndup(o, rec->len - (o - rec->str), -1));
		return -1;
	} else if ((size_t)(o - rec->str) > rec->len) {
		PMSG("o - str=%u,len=%u:", (o - rec->str), rec->len);
		hexdump(stdout, rec->str, 64, 16);
		return -1;
	}
	o = mbsnoff(rec->str, -1, rec->len);
	if ((size_t)(o - rec->str) < rec->len) {
		PMSG("o - str=%u,len=%u,o=[%s]", (o - rec->str), rec->len, mbsndup(o, rec->len - (o - rec->str), -1));
		return -1;
	} else if ((size_t)(o - rec->str) > rec->len) {
		PMSG("o - str=%u,len=%u,over:", (o - rec->str), rec->len);
		hexdump(stdout, rec->str + rec->len, o - (rec->str + rec->len), 16);
		return -1;
	}

	n = rec->nchars;
	o = rec->str;
	while (n--) {
		/* hexdump(stdout, o, (rec->str + rec->len) - o, 16); */
		mbtowc(&wc, o, -1);
		if ((s = mbsnchr(rec->str, -1, rec->nchars, wc)) != o) {
			;
		}
		o = mbsnoff(o, 1, -1);
	}
	if ((size_t)(o - rec->str) < rec->len) {
		PMSG("o - str=%u,len=%u,o=[%s]", (o - rec->str), rec->len, mbsndup(o, rec->len - (o - rec->str), -1));
		return -1;
	} else if ((size_t)(o - rec->str) > rec->len) {
		PMSG("o - str=%u,len=%u,over:", (o - rec->str), rec->len);
		hexdump(stdout, rec->str + rec->len, o - (rec->str + rec->len), 16);
		return -1;
	}

	return 0;
}

size_t
record_read(struct record *rec, char *src)
{
	const char *start;

	start = src;

	rec->nchars = strtol(src, NULL, 10);

	while (*src++ != '[') {
		;
	}

	rec->str = src;

	while (src[0] != ']' || src[1] != '\n') {
		src++;
	}

	rec->len = src - rec->str;

	return src - start + 2;
}
int
MbsSimple(int verbose, struct cfg *cfg, char *args[])
{
	char fbuf[0xFFFF];
	FILE *fd;
	size_t flen, n;
	struct record rec;
	int rn;
	cfg = NULL;

	if (!setlocale(LC_CTYPE, "en_US.UTF-8")) {
			fprintf(stderr, "Cannot set UTF-8 locale.");
		return -1;
	}

	errno = 0;

	rn = atoi(args[1]);
	fd = fopen(args[0], "r");
	flen = fread(fbuf, 1, 0xFFFF, fd);

	n = 0;
	while (n < flen) {
		n += record_read(&rec, fbuf + n);
		if (rn-- == 0) {
			if (run(verbose, &rec) == -1) {
				AMSG("");
				return -1;
			}
		}
	}

	fclose(fd);

	tcase_printf(verbose, "done");
    return 0;
}
