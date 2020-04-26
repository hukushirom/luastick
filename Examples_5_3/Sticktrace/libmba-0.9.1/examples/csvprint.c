#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <mba/msgno.h>
#include <mba/csv.h>

#define BUF_MAX 0x7FFF
#define ROW_MAX 0xFFF

int
run(const char *filename, const char *format, int filter, int count, int sep)
{
	FILE *in;
	unsigned char buf[BUF_MAX], *row[ROW_MAX];
	int n, flags = CSV_TRIM | CSV_QUOTES;

	if ((in = fopen(filename, "r")) == NULL) {
		PMNF(errno, ": %s", filename);
		return -1;
	}

	if (sep == '\t') {
		flags &= ~CSV_QUOTES; /* do not interpret quotes with tab delimited files */
	}

	while ((n = csv_row_fread(in, buf, BUF_MAX, row, ROW_MAX, sep, flags)) > 0) {
		const char *fmt = format;
		char outbuf[BUF_MAX];
		char *out = outbuf;

		if (filter && strcmp(row[0], "0") == 0) {
			continue;
		}

		while (*fmt) {
			if (*fmt == '\\') {
				fmt++;
				switch (*fmt) {
					case '\0':
						return -1;
					case 'n':
						*out++ = '\n';
						break;
					case 't':
						*out++ = '\t';
						break;
					case 'r':
						*out++ = '\r';
						break;
					default:
						*out++ = *fmt;
				}
				fmt++;
			} else if (*fmt == '%') {
				unsigned long i;
				char *endptr;
				fmt++;

				if (*fmt == 'c') {
					int n;
					if ((n = sprintf(out, "%d", count)) == -1) {
						PMNO(errno);
						return -1;
					}
					out += n;
					fmt++;
					continue;
				}

				if ((i = strtoul(fmt, &endptr, 10)) == ULONG_MAX) {
					PMNF(errno, ": %s", fmt);
					return -1;
				}
				fmt = endptr;
				if (i < ROW_MAX) {
					const char *s = row[i];
					if (s) {
						while (*s) {
							*out++ = *s++;
						}
					} else {
						*out++ = '-';
					}
				}
			} else {
				*out++ = *fmt++;
			}
		}
		*out = '\0';

		count++;

		fputs(outbuf, stdout);
	}
	if (n == -1) {
		AMSG("");
		return -1;
	}

	fclose(in);

	return 0;
}

int
main(int argc, char *argv[])
{
	char **args;
	char *filename = NULL;
	char *format = NULL;
	int filter = 0;
	unsigned long count = 1;
	int sep = ',';

	if (argc < 3) {
usage:
		fprintf(stderr, "usage: %s [-f] [-s <sep>] <filename> <format>\n", argv[0]);
		return EXIT_FAILURE;
	}

	errno = 0;

	args = argv;
	args++; argc--;

	while (argc) {
		if (strcmp(*args, "-f") == 0) {
			filter = 1;
		} else if (strcmp(*args, "-c") == 0) {
			args++; argc--;
			if ((count = strtoul(*args, NULL, 0)) == ULONG_MAX) {
				fprintf(stderr, "invalid count value: %s", *args);
				goto usage;
			}
		} else if (strcmp(*args, "-s") == 0) {
			args++; argc--;
			sep = **args;
			if (sep == '\\') {
				sep = *(*args + 1);
				switch (sep) {
					case 't':
						sep = '\t';
						break;
				}
			}
		} else if (filename) {
			if (format) {
				fprintf(stderr, "invalid argument: %s\n", *args);
				goto usage;
			}
			format = *args;
		} else {
			filename = *args;
		}
		args++; argc--;
	}

	if (run(filename, format, filter, count, sep) == -1) {
		MMSG("Run failed.");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
