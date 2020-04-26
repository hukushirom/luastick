#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <mba/text.h>
#include <mba/linkedlist.h>
#include <mba/hexdump.h>
#include <mba/msgno.h>

#define DEFAULT_WIDTH 16
#define TYPE_FRAGMENTED 0
#define TYPE_HTML 1
#define COLOR_START 8
#define COLOR_STEP  5

#define MIN(a,b) ((a) < (b) ? (a) : (b))

const char *colors[] = { "#000000", "#000080", "#800000" };

struct range {
	size_t off;
	size_t len;
	unsigned char *remark;
	unsigned char bgcolor;
	unsigned char border;
	unsigned char color;
};
struct cell {
	unsigned char val;
	unsigned char bgcolor;
	unsigned char border;
	unsigned char color;
};

int
mkcells(struct cell *cells, const unsigned char *src, size_t n, struct linkedlist *ranges)
{
	iter_t iter;
	struct range *range;
	size_t off;
	unsigned char bgcolor, border, color;
	int level;
	size_t ends[3];

	for (off = 0; off < n; off++) {
		cells[off].val = src[off];
	}

	bgcolor = border = color = 1;

	level = 0;
	ends[level] = 0;

	linkedlist_iterate(ranges, &iter);
	while ((range = linkedlist_next(ranges, &iter))) {
		off = range->off;
		if (off < ends[level]) {
			level++;
		} else {
			while (level) {
				if (off < ends[level - 1]) {
					break;
				}
				level--;
			}
		}
		ends[level] = range->off + range->len;
		while (off < ends[level]) {
			switch (level) {
				case 0:
					cells[off].bgcolor = bgcolor;
					break;
				case 1:
					cells[off].border = border;
					break;
				case 2:
					cells[off].color = color;
					break;
			}
			off++;
		}
		switch (level) {
			case 0:
				range->bgcolor = bgcolor;
				bgcolor++;
				break;
			case 1:
				range->border = border;
				border++;
				break;
			case 2:
				range->color = color;
				color++;
				break;
		}
	}

	return 0;
}
unsigned long
colors_get(unsigned long index, int dark)
{
	unsigned long idx = index * 32, color;
	unsigned char val = idx & 0xFF;
	int rgb = (idx >> 8) % 3;

	if (rgb == 0) {
		color = 0xFF0000 | (val << 8) | (0xFF - val);
	} else if (rgb == 1) {
		color =  ((0xFF - val) << 16) | 0x00FF00 | val;
	} else {
		color = (val << 16) | ((0xFF - val) << 8) | 0x0000FF;
	}

	if (dark) {
		color = ~color & 0xFFFFFF;
	}

	return color;
}
void
print_style()
{
	int i, ci;

	ci = COLOR_START;
	for (i = 1; i < 25; i++) {
		unsigned long color = colors_get(ci, 0);
		ci += COLOR_STEP;
		fprintf(stdout, ".bg%d { background-color: #%06lx; }\n", i, color);
	}
	for (i = 1; i < 25; i++) {
		unsigned long color = colors_get(ci, 1);
		ci += COLOR_STEP;
		fprintf(stdout, ".bo%d { border: 2px #%06lx solid; }\n", i, color);
	}
	for (i = 1; i < 25; i++) {
		fprintf(stdout, ".co%d { font-weight: bold; color: %s; }\n", i, colors[i % 3]);
	}
}
int
print_tail(FILE *stream, struct cell *cells, size_t n, size_t width)
{
	size_t off;
	struct cell *c;

	fputs("<td>&nbsp;|", stream);
	for (off = 0; off < n; off++) {
		c = cells + off;
		fprintf(stream, "<td class=\"bg%d bo%d co%d\">", c->bgcolor, c->border, c->color);
		if (isprint(c->val)) {
			fprintf(stream, "%c", c->val);
		} else {
			fputc('.', stream);
		}
	}
	while (off++ < width) {
		fputs("<td>&nbsp;", stream);
	}
	fputs("<td>|\n", stream);

	return 0;
}
int
hexdump_html_cells(FILE *stream, struct cell *cells, size_t n, int width)
{
	size_t off;
	struct cell *c;

	off = 0;
	fprintf(stream, "<tr><td>%05x:&nbsp;", off);
	while (off < n) {
		c = cells + off;
		fprintf(stream, "<td class=\"bg%d bo%d co%d\">%02x&nbsp;",
				c->bgcolor, c->border, c->color, c->val);
		off++;
		if ((off % width) == 0) {
			print_tail(stream, cells + (off - width), width, width);
			if (off < n) {
				fprintf(stream, "<tr><td>%05x:&nbsp;", off);
			}
		}
	}
	n = off % width;
	if (n) {
		int td = width - n;
		while (td--) {
			fputs("<td>&nbsp;", stream);
		}
		print_tail(stream, cells + (off - n), n, width);
	}

	return 0;
}
int
hexdump_html(FILE *stream, const void *src, size_t n, int width, struct linkedlist *ranges)
{
	struct cell *cells;
	iter_t iter;
	struct range *range;

	if ((cells = calloc(n, sizeof *cells)) == NULL) {
		PMNO(errno);
		return -1;
	}

	if (mkcells(cells, src, n, ranges) == -1) {
		AMSG("");
		return -1;
	}

	fputs("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n", stream);
	fputs("<html><head>\n", stream);
	fputs("<STYLE TYPE=\"text/css\">\n", stream);

	print_style();

    fputs("</STYLE>\n", stream);
	fputs("</head><body>\n", stream);

	fputs("<table style=\"font-family: monospaced, courier;\"><tr><td valign=\"top\">\n", stream);

	fputs("\n<table style=\"font-size: small; border-collapse: collapse;\">\n", stream);
	if (hexdump_html_cells(stream, cells, n, width) == -1) {
		AMSG("");
		return -1;
	}
	fputs("\n</table>\n", stream);

	fputs("<td valign=\"top\">\n", stream);

	fputs("<div style=\"font-size: x-small;\"><table style=\"border-collapse: collapse;\">\n", stream);
	linkedlist_iterate(ranges, &iter);
	fprintf(stream, "<tr><th>key<th><th>offset<th>length<th>remark\n");
	while ((range = linkedlist_next(ranges, &iter))) {
		unsigned char *remark = range->remark ? range->remark : (unsigned char *)"";
		fprintf(stream, "<tr><td class=\"bg%d bo%d co%d\">00<td>&nbsp;<td>0x%05x<td align=\"right\">%d<td>%s\n",
				range->bgcolor, range->border, range->color,
				range->off, range->len, remark);
	}
	fputs("</table></div>\n", stream);

	fputs("</table>\n", stream);

	fputs("</body></html>\n", stream);

	free(cells);

	return 0;
}
int
hexdump_fragmented(FILE *stream, const void *src, size_t n, int width, struct linkedlist *ranges)
{
	if (linkedlist_size(ranges) == 0) {
		hexdump(stream, src, n, width);
	} else {
		iter_t iter;
		struct range *range;

		linkedlist_iterate(ranges, &iter);
		while ((range = linkedlist_next(ranges, &iter))) {
			size_t len;

			if (range->off >= n) {
				break;
			}
			len = MIN(n - range->off, range->len);
			if (range->remark) {
				fputs(range->remark, stream);
				fputc('\n', stream);
			}
			hexdump(stream, (char *)src + range->off, len, width);
		}
	}
	return 0;
}
void *
mapfile(const unsigned char *filename, int *size)
{
	void *ret = NULL;
	int fd;
	struct stat st;

	if ((fd = open(filename, O_RDONLY)) == -1 || fstat(fd, &st) == -1) {
		PMNO(errno);
		return NULL;
	}
	if ((ret = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
		PMNO(errno);
		return NULL;
	}

	*size = st.st_size;

	return ret;
}
static int
rangecmp(const struct range *r1, const struct range *r2, void *context)
{
	if (r1->off == r2->off) {
		if (r1->len == r2->len) {
			return 0;
		} else if (r1->len > r2->len) {
			return -1;
		}
		return 1;
	} else if (r1->off < r2->off) {
		return -1;
	}
	(void)context;
	return 1;
}
int
add_range(struct linkedlist *ranges, unsigned char *fields[], unsigned char *flim, int fn, size_t *off)
{
	struct range *range;
	char *end;

	if ((range = calloc(1, sizeof *range)) == NULL) {
		PMNO(errno);
		return -1;
	}

	switch (fn) {
		case 3:
			if ((range->off = strtoul(fields[0], &end, 0)) == ULONG_MAX) {
				PMNO(errno);
				return -1;
			}
			if ((range->len = strtoul(fields[1], &end, 0)) == 0 ||
						range->len == ULONG_MAX) {
				PMSG("invalid range length: %s", fields[1]);
				return -1;
			}
			if (*(fields[2]) == '\0') {
				range->remark = NULL;
			} else if (str_copy_new(fields[2], flim, &range->remark, -1, NULL) == -1) {
				AMSG("");
				return -1;
			}
			break;
		case 2:
			range->off = -1;
			if ((range->len = strtoul(fields[0], &end, 0)) == 0 ||
						range->len == ULONG_MAX) {
				PMSG("invalid range length: %s", fields[0]);
				return -1;
			}
			if (*(fields[1]) == '\0') {
				range->remark = NULL;
			} else if (str_copy_new(fields[1], flim, &range->remark, -1, NULL) == -1) {
				AMSG("");
				return -1;
			}
			break;
		case 1:
			range->off = -1;
			if ((range->len = strtoul(fields[0], &end, 0)) == 0 ||
						range->len == ULONG_MAX) {
				PMSG("invalid range length: %s", fields[0]);
				return -1;
			}
			range->remark = NULL;
			break;
	}
	if (range->off == (size_t)-1) {
		range->off = *off;
	}
	*off = range->off + range->len;

	if (linkedlist_insert_sorted(ranges, (cmp_fn)rangecmp, NULL, NULL, range) == -1) {
		AMSG("");
		free(range);
		return -1;
	}

	return 0;
}
int
parse_ranges(struct linkedlist *ranges, const unsigned char *input)
{
	const unsigned char *ip = input;
	unsigned char buf[BUFSIZ], *next, *bp;
	unsigned char *fields[3];
	int fi;
	size_t off = 0;

	if (!input) return 0;

	/* eg: "10,20,2:8:remark,30,2:9:"
	 */

	next = bp = buf;
	fi = 0;

	for ( ;; ) {
		if (*ip == ':' || *ip == ',' || *ip == '\0') {
			fields[fi++] = next;
			*bp++ = '\0';
			next = bp;

			if (*ip == ',' || *ip == '\0') {
				if (add_range(ranges, fields, buf + BUFSIZ, fi, &off) == -1) {
					AMSG("");
					return -1;
				}

				next = bp = buf;
				fi = 0;

				if (*ip == '\0') {
					break;
				}
			}
		} else {
			if (*ip == '\\' && *(++ip) == '\0') {
				continue;
			}
			*bp++ = *ip;
		}
		ip++;
	}
	return 0;
}
int
run(const unsigned char *filename, int width, int type, const unsigned char *_ranges)
{
	void *mem;
	int n;
	struct linkedlist ranges;

	if ((mem = mapfile(filename, &n)) == NULL ||
				linkedlist_init(&ranges, 0, NULL) == -1 ||
				parse_ranges(&ranges, _ranges) == -1) {
		AMSG("");
		return -1;
	}

	if (type == TYPE_FRAGMENTED) {
		if (hexdump_fragmented(stdout, mem, n, width, &ranges) == -1) {
			AMSG("");
			return -1;
		}
	} else if (type == TYPE_HTML) {
		if (hexdump_html(stdout, mem, n, width, &ranges) == -1) {
			AMSG("");
			return -1;
		}
	} else {
		PMSG("Invalid output type");
		return -1;
	}

	return 0;
}
int
main(int argc, char *argv[])
{
	char **args;
	char *filename = NULL;
	unsigned long width = DEFAULT_WIDTH;
	int type = TYPE_FRAGMENTED;
	char *ranges = NULL;

	if (argc < 2) {
usage:
		fprintf(stderr, "usage: %s [-h] [-r <ranges>] [-<width>] <file>\n", argv[0]);
		return EXIT_FAILURE;
	}

	args = argv;
	args++; argc--;

	while (argc) {
		if (strcmp(*args, "-h") == 0) {
			type = TYPE_HTML;
		} else if (strcmp(*args, "-r") == 0) {
			args++; argc--;
			ranges = *args;
		} else if (**args == '-') {
			char *end;
			if ((width = strtoul((*args) + 1, &end, 0)) == ULONG_MAX) {
				MMNO(errno);
				goto usage;
			}
		} else if (filename) {
			MMSG("filename already specified");
			goto usage;
		} else {
			filename = *args;
		}
		args++; argc--;
	}

	if (run(filename, width, type, ranges) == -1) {
		MMSG("");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

