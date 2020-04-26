/* mktool - A Makefile Tool
 * Copyright (c) 2005 Michael B. Allen <mba2000 ioplex.com>
 *
 * The MIT License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>

#define ERR(e,m) fprintf(stderr, "line %d: %s: %s\n", __LINE__, strerror(e), m);

#define BSIZ 8192

#define ARG_CC     0
#define ARG_DEBUG  1
#define ARG_WARN1  2
#define ARG_WARN2  3
#define ARG_C99    4
#define ARG_C89    5
#define ARG_PIC1   6
#define ARG_PIC2   7
#define ARG_SONAME 8
#define ARG_SOVERS 9
#define ARG_SHARED 10
#define ARG_RPATH  11
#define ARG_SOOUT  12
#define ARG_SOLNK1 13
#define ARG_SOLNK2 14
#define ARG_PLATFO 15

static const char *arg_array[32] = {
#if defined(__GNUC__)
#if defined(linux)
		"gcc",
		" -g",
		" -Wall -W",
		" -Wall -W -ansi -pedantic",
		" -std=c99",
		" -std=c89",
		" -fpic",
		" -fPIC",
		" -Wl,-soname,lib%n.so.%1.%2",
		"",
		" -shared",
		" -Wl,-rpath,%s",
		"lib%n.so.%1.%2.%3",
		"lib%n.so.%1.%2",
		"lib%n.so",
		"linux",
#elif defined(__APPLE__) && defined(__MACH__)
		"gcc",
		" -g",
		" -Wall -W",
		" -Wall -W -ansi -pedantic",
		" -std=c99",
		" -std=c89",
		" -fPIC",
		" -fPIC",
		" -install_name lib%n.%1.%2.dylib",
		" -current_version %1.%2.%3",
		" -dynamiclib",
		"",
		"lib%n.%1.%2.%3.dylib",
		"lib%n.%1.%2.dylib",
		"lib%n.dylib",
		"darwin",
#elif defined(__FreeBSD__) || defined(__NetBSD__)
		"gcc",
		" -g",
		" -Wall -W",
		" -Wall -W -ansi -pedantic",
		" -std=c99",
		" -std=c89",
		" -fpic",
		" -fPIC",
		" -Wl,-soname,lib%n.so.%1.%2",
		"",
		" -shared",
		" -Wl,-rpath,%s",
		"lib%n.so.%1",
		"",
		"lib%n.so",
		"bsd",
#elif defined(hpux)
		"gcc",
		" -g",
		" -Wall -W",
		" -Wall -W -ansi -pedantic",
		" -std=c99",
		" -std=c89",
		" -fpic",
		" -fPIC",
		"",
		"",
		" -shared",
		" -Wl,+b,%s",
		"lib%n.%1.sl",
		"",
		"lib%n.sl",
		"hpux",
#else
#error unknown platform
#endif
#elif defined(__DECC)
		"cc",
		" -g",
		"",
		"",
		" -c99",
		" -std1",
		"",
		"",
		" -soname lib%n.so.%1.%2",
		"",
		" -shared",
		" -rpath %s",
		"lib%n.%1.%2.%3.so",
		"lib%n.%1.%2.so",
		"lib%n.so",
		"osf1",
#elif defined(__HP_aCC) || defined(__HP_cc)
		"cc",
		" -g",
		"",
		" +w",
		" -AC99",
		" -AC89",
		" +z",
		" +Z",
		"",
		"",
		" -b",
		" -Wl,+b,%s",
		"lib%n.%1.sl",
		"",
		"lib%n.sl",
		"hpux",
#else
#error unknown compiler
#endif
		NULL
};

const char *
escape_quotes(const char *str)
{
	static char buf[8192];
	char *bp = buf, *blim = buf + 8192;

	while (*str && bp < blim) {
		if (*str == '"') {
			*bp++ = '\\';
		}
		*bp++ = *str++;
	}
	if (bp == blim) {
		return "error";
	}
	*bp = '\0';

	return buf;
}
int
mkcccmd(char *buf, int argc, char *argv[])
{
	char *bp = buf;
	char **args = argv;

	bp += sprintf(bp, arg_array[ARG_CC]);

	while (argc) {
		if (strcmp(*args, "-g") == 0) {
			bp += sprintf(bp, arg_array[ARG_DEBUG]);
		} else if (strcmp(*args, "-W1") == 0) {
			bp += sprintf(bp, arg_array[ARG_WARN1]);
		} else if (strcmp(*args, "-W2") == 0) {
			bp += sprintf(bp, arg_array[ARG_WARN2]);
		} else if (strncmp(*args, "-std=", 5) == 0) {
			static const char *stds[] = { "c99", "c89", NULL };
			int std = 0;

			while (stds[std] && strcmp(*args + 5, stds[std])) {
				std++;
			}
			if (stds[std] != NULL) {
				bp += sprintf(bp, arg_array[ARG_C99 + std]);
			} else { /* just copy */
				fprintf(stderr, "Argument '%s' not translated - just copying ...\n", *args);
				bp += sprintf(bp, " %s", *args);
			}
		} else if (strcmp(*args, "-fpic") == 0) {
			bp += sprintf(bp, arg_array[ARG_PIC1]);
		} else if (strcmp(*args, "-fPIC") == 0) {
			bp += sprintf(bp, arg_array[ARG_PIC2]);
		} else if (strcmp(*args, "-rpath") == 0) {
			args++; argc--;
			if (!argc) {
				ERR(EINVAL, "-rpath requires an argument");
				return -1;
			}
			bp += sprintf(bp, arg_array[ARG_RPATH], *args);
		} else {
			/* Just copy the argument */
			bp += sprintf(bp, " %s", escape_quotes(*args));
		}
		args++; argc--;
	}

	return bp - buf;
}
/* Examples:
 * lib%n.so.%1.%2 -> libfoo.so.0.8
 * lib%n.%1.%2.%3.dylib -> libfoo.0.8.25.dylib
 */
int
slibprintf(char *str, const char *fmt, const char *name, char *vers[])
{
	char *start = str;

	for ( ; *fmt; fmt++) {
		if (*fmt == '%') {
			fmt++;
			switch (*fmt) {
				case 'n':
					str += sprintf(str, name);
					break;
				case '1':
					str += sprintf(str, vers[0]);
					break;
				case '2':
					str += sprintf(str, vers[1]);
					break;
				case '3':
					str += sprintf(str, vers[2]);
					break;
				case '%':
					*str++ = '%';
					break;
				default:
					fprintf(stderr, "invalid conversion specifier '%%%c' - valid conversion "
							"specifiers are %%n, %%1, %%2, and %%3 for libname and major, "
							"minor, micro version numbers respectively\n", *fmt);
					return 0;
			}
		} else {
			*str++ = *fmt;
		}
	}
	*str = '\0';

	return str - start;
}
int
mklibcmd(char *buf, const char *libname, char *vers[], int argc, char *argv[])
{
	char *bp = buf;
	char **args = argv;

#if defined(__GNUC__)
	bp += sprintf(bp, "gcc");
#else
	bp += sprintf(bp, "cc");
#endif
	while (argc) {
		if (strcmp(*args, "-soname") == 0) {
			bp += slibprintf(bp, arg_array[ARG_SONAME], libname, vers);
		} else if (strcmp(*args, "-sovers") == 0) {
			bp += slibprintf(bp, arg_array[ARG_SOVERS], libname, vers);
		} else if (strcmp(*args, "-shared") == 0) {
			bp += sprintf(bp, arg_array[ARG_SHARED]);
		} else {
			/* Just copy the argument */
			bp += sprintf(bp, " %s", *args);
		}
		args++; argc--;
	}

	bp += sprintf(bp, " -o ");
	bp += slibprintf(bp, arg_array[ARG_SOOUT], libname, vers);

	return bp - buf;
}

int
mkdirs(const char *dir, mode_t mode)
{
	char buf[PATH_MAX + 1], *bp = buf, *blim = buf + PATH_MAX;
	const char *dp = dir;

	while (*dp && bp < blim) {
		*bp++ = *dp++;
		if (*dp == '/' || *dp == '\0') {
			*bp = '\0';
			if (mkdir(buf, mode) == -1 && errno != EEXIST) {
				ERR(errno, buf);
				return -1;
			}
		}
	}

	return 0;
}
int
cpfile(const char *file, const char *dir, mode_t mode)
{
	char buf[BSIZ + 1], *bp = buf, *blim = buf + BSIZ;
	const char *dp = dir, *fname = file;
	int src, dst, n = 1;

	if ((src = open(file, O_RDONLY)) == -1) {
		ERR(errno, file);
		return -1;
	}

	while (bp < blim && *dp) {        /* copy dir */
		*bp++ = *dp++;
		n = *dp == '/';
	}
	if (!n) *bp++ = '/'; /* assure trailing slash */
	dp = fname;      /* find beginning of filename */
	while (*dp) {
		if (*dp++ == '/') {
			fname = dp;
		}
	}
	while (bp < blim && *fname) { /* copy filename */
		*bp++ = *fname++;
	}
	*bp = '\0';
	if ((dst = open(buf, O_RDWR | O_CREAT | O_TRUNC, mode)) == -1) {
		ERR(errno, buf);
		return -1;
	}

	while ((n = read(src, buf, BSIZ)) > 0) {
		while (n) {
			int m;
			if ((m = write(dst, buf, n)) == -1) {
				ERR(errno, file);
				return -1;
			}
			n -= m;
		}
	}
	if (n == -1) {
		ERR(errno, file);
		return -1;
	}

	return 0;
}
int
doinstcmd(const char *dir, int argc, char *argv[], mode_t mode)
{
	int i;

	umask(0);

	if (mkdirs(dir, 0755) == -1) {
		return -1;
	}
	for (i = 0; i < argc; i++) {
		if (cpfile(argv[i], dir, mode) == -1) {
			return -1;
		}
	}

	return 0;
}
int
rmfile(const char *dir, const char *file)
{
	char path[PATH_MAX + 1], *bp = path, *blim = path + PATH_MAX;
	const char *dp = dir;

	if (dir) {
		int n = 1;
		while (bp < blim && *dp) {        /* copy dir */
			n = *dp == '/';
			*bp++ = *dp++;
		}
		if (!n) *bp++ = '/'; /* assure trailing slash */
	}

	dp = file;          /* find beginning of filename */
	while (*dp) {
		if (*dp++ == '/') {
			file = dp;
		}
	}
	while (bp < blim && *file) {     /* copy filename */
		*bp++ = *file++;
	}
	*bp = '\0';

	if (bp == blim) {
		ERR(ENOBUFS, file);
		return -1;
	}

	unlink(path);

	return 0;
}
int
doinst(const char *libname, char *vers[], int argc, char *argv[])
{
	mode_t mode = 0;
	int i;

	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-m") == 0) {
			unsigned long ul;
			if (++i == argc) return -2;
			if ((ul = strtoul(argv[i], NULL, 8)) == ULONG_MAX) {
				ERR(errno, argv[i]);
				return -1;
			}
			mode = (mode_t)ul;
		} else {
			break;
		}
	}
	if (i == argc) return -1;

	if (libname) {
		char fname[255], *arg[] = { fname }, lname[255];

		slibprintf(fname, arg_array[ARG_SOOUT], libname, vers);
		if (doinstcmd(argv[argc - 1], 1, arg, mode ? mode : 0755) == -1) {
			return -1;
		}
		if (chdir(argv[argc - 1]) == -1) {
			ERR(errno, argv[argc - 1]);
			return -1;
		}
		if (arg_array[ARG_SOLNK1][0]) {
			slibprintf(lname, arg_array[ARG_SOLNK1], libname, vers);
			symlink(fname, lname);
		}
		if (arg_array[ARG_SOLNK2][0]) {
			slibprintf(lname, arg_array[ARG_SOLNK2], libname, vers);
			symlink(fname, lname);
		}
	} else {
		if (doinstcmd(argv[argc - 1], argc - i - 1, argv + i, mode ? mode : 0644) == -1) {
			return -1;
		}
	}

	return 0;
}

int
main(int argc, char *argv[])
{
	char buf[BSIZ];
	int cmd, i = 2, verbose = 0;

	if (argc < 2) {
usage:
		fprintf(stderr, "%s -c [-v] [-g] [-W1|-W2] [-std=xxx] [-fpic|-fPIC] [-rpath <path>]"
				"<other arguments>\n", argv[0]);
		fprintf(stderr, "%s -l [-v] -libname <name> -libvers <vers> [-shared] [-soname] [-sovers] "
				"<other arguments>\n", argv[0]);
		fprintf(stderr, "%s -i [-m <mode>] <file1> <file2> <fileN> <dir>\n", argv[0]);
		fprintf(stderr, "%s -i -libname <name> -libvers <vers> [-m mode] <dir>\n", argv[0]);
		fprintf(stderr, "%s -u <dir> <file1> <file2> <fileN>\n", argv[0]);
		fprintf(stderr, "%s -u -libname <name> -libvers <vers> <srcdir>\n", argv[0]);
		fprintf(stderr, "%s -m <target>\n", argv[0]);
		return EXIT_FAILURE;
	}

	cmd = argv[1][1];
	if (argc > 2 && strcmp(argv[2], "-v") == 0) {
		verbose = 1;
		i++;
	}

	switch (cmd) {
		case 'c':
			if (argc < 3) {
				fprintf(stderr, "no compiler arguments provided\n");
				goto usage;
			}
			if (mkcccmd(buf, argc - i, argv + i) == -1) {
				fprintf(stderr, "failed to build command\n");
				return EXIT_FAILURE;
			}
			break;
		case 'l':
		case 'i':
		case 'u':
		case 'C':
			{
				const char *libname = NULL, *dir = NULL;
				char vbuf[255], *vers[3] = { NULL, NULL, NULL };

				for ( ; i < argc; i++) {
					if (strcmp(argv[i], "-libname") == 0) {
						if (++i == argc) goto usage;
						libname = argv[i];
					} else if (strcmp(argv[i], "-libvers") == 0) {
						char *vp = vbuf;
						int vi = 0;

						if (++i == argc) goto usage;
						strcpy(vbuf, argv[i]);
						vers[vi++] = vp;
						while (vi < 3 && *vp++) {
							if (*vp == '.') {
								*vp++ = '\0';
								vers[vi++] = vp;
							}
						}
					} else {
						break;
					}
				}
				if (i == argc && cmd != 'C') {
					ERR(EINVAL, "more arguments expected");
					goto usage;
				}

				switch (cmd) {
					case 'l':
						if (mklibcmd(buf, libname, vers, argc - i, argv + i) == -1) {
							return EXIT_FAILURE;
						}
						break;
					case 'i':
						if (doinst(libname, vers, argc - i, argv + i) == -1) {
							return EXIT_FAILURE;
						}
						return EXIT_SUCCESS;
					case 'u':
						dir = argv[argc - 1];
						argc--;
					case 'C':
						if (libname) {
							if (cmd != 'C') {
								if (arg_array[ARG_SOLNK2][0]) {
									slibprintf(buf, arg_array[ARG_SOLNK2], libname, vers);
									if (rmfile(dir, buf) == -1 && errno != ENOENT) return EXIT_FAILURE;
								}
								if (arg_array[ARG_SOLNK1][0]) {
									slibprintf(buf, arg_array[ARG_SOLNK1], libname, vers);
									if (rmfile(dir, buf) == -1 && errno != ENOENT) return EXIT_FAILURE;
								}
							}
							slibprintf(buf, arg_array[ARG_SOOUT], libname, vers);
							if (rmfile(dir, buf) == -1 && errno != ENOENT) return EXIT_FAILURE;
						} else {
							for ( ; i < argc; i++) {
								if (rmfile(dir, argv[i]) == -1 && errno != ENOENT) {
									return EXIT_FAILURE;
								}
							}
						}

						return EXIT_SUCCESS;
				}
			}
			break;
		case 'm':
			{
				char *bp = buf;
				bp += sprintf(bp, "make -f Makefile.%s", arg_array[ARG_PLATFO]);
				if (i < argc) {
					bp += sprintf(bp, " %s", argv[i]);
				}
			}
			break;
		default:
			fprintf(stderr, "first argument must be a command flag\n");
			goto usage;
	}

	/* run command */
	
	if (verbose) {
		fprintf(stderr, "%s\n", buf);
	}

	if (system(buf) == -1) {
		perror("system");
		return -1;
	}

	return EXIT_SUCCESS;
}

