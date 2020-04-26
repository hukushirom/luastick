#include <time.h>
#include "common.h"
#include "mba/text.h"

static int
_t(int line)
{
	fprintf(stderr, "failure line: %d\n", line);
	return -1;
}

#define T(cond) if ((cond) == 0) return _t(__LINE__)

int
TextTest(int verbose, struct cfg *cfg, char *args[])
{
	tchar buf[1024];
	tchar *hello = TEXT("hello");
	tchar *a = TEXT("a");
	tchar *e = TEXT("");
	tchar *cpy;
	cfg = NULL; *args = NULL; verbose = 1;

                                        /* text_length */

	T(text_length(NULL, NULL) == 0);
	T(text_length(hello, NULL) == 0);
	T(text_length(hello + 3, hello) == 0);
	T(text_length(hello, hello) == 0);
	T(text_length(hello, hello + 1) == 0);
	T(text_length(hello, hello + 4) == 0);
	T(text_length(a, a + 1) == 0);
	T(text_length(a, a + 2) == 1);
	T(text_length(a + 1, a + 2) == 0);
	T(text_length(hello, hello + 100) == 5);

                                          /* text_size */

	T(text_size(NULL, NULL) == 0);
	T(text_size(hello, NULL) == 0);
	T(text_size(hello + 3, hello) == 0);
	T(text_size(hello, hello) == 0);
	T(text_size(hello, hello + 1) == 0);
	T(text_size(hello, hello + 4) == 0);
	T(text_size(a, a + 1) == 0);
	T(text_size(a, a + 2) == (2 * sizeof *a));
	T(text_size(a + 1, a + 2) == (1 * sizeof *a));
	T(text_size(hello, hello + 100) == (6 * sizeof *hello));

                                          /* text_copy */

	T(text_copy(hello, hello + 100, NULL, buf + 1024, 0) == 0);

	T(text_copy(hello, hello + 100, buf + 3, buf, 0) == 0);

	memset(buf, 'x', 1024 * sizeof(tchar));
	T(text_copy(NULL, hello + 100, buf, buf + 1024, 0) == 0);
	T(buf[0] == TEXT('\0'));

	memset(buf, 'x', 1024 * sizeof(tchar));
	T(text_copy(hello + 3, hello, buf, buf + 1024, 0) == 0);
	T(buf[0] == TEXT('\0'));

	memset(buf, 'x', 1024 * sizeof(tchar));
	T(text_copy(hello, hello + 100, buf, buf + 100, 1) == 1);
	T(tcsncmp(TEXT("h"), buf, 100) == 0);

	memset(buf, 'x', 1024 * sizeof(tchar));
	T(text_copy(hello, hello + 100, buf, buf + 100, 5) == 5);
	T(tcsncmp(hello, buf, 100) == 0);

	memset(buf, 'x', 1024 * sizeof(tchar));
	T(text_copy(hello, hello + 100, buf, buf + 1024, 0) == 0);
	T(buf[0] == TEXT('\0'));

	memset(buf, 'x', 1024 * sizeof(tchar));
	T(text_copy(e, e + 100, buf, buf + 1024, 100) == 0);
	T(tcsncmp(e, buf, 100) == 0);

	memset(buf, 'x', 1024 * sizeof(tchar));
	T(text_copy(hello, hello + 100, buf, buf + 1024, 100) == 5);
	T(tcsncmp(hello, buf, 100) == 0);

	memset(buf, 'x', 1024 * sizeof(tchar));
	T(text_copy(hello, hello + 1, buf, buf + 1024, 100) == 0);
	T(buf[0] == TEXT('\0'));

	memset(buf, 'x', 1024 * sizeof(tchar));
	T(text_copy(hello, hello + 5, buf, buf + 1024, 100) == 0);
	T(buf[0] == TEXT('\0'));

	memset(buf, 'x', 1024 * sizeof(tchar));
	T(text_copy(hello, hello + 6, buf, buf + 1024, 100) == 5);
	T(tcsncmp(hello, buf, 100) == 0);

	memset(buf, 'x', 1024 * sizeof(tchar));
	T(text_copy(hello, hello + 100, buf, buf + 1, 100) == 0);
	T(buf[0] == TEXT('\0'));

	memset(buf, 'x', 1024 * sizeof(tchar));
	T(text_copy(hello, hello + 100, buf, buf + 5, 100) == 0);
	T(buf[0] == TEXT('\0'));

	memset(buf, 'x', 1024 * sizeof(tchar));
	T(text_copy(hello, hello + 100, buf, buf + 6, 100) == 5);
	T(tcsncmp(hello, buf, 100) == 0);

                                      /* text_copy_new */

	T(text_copy_new(hello, hello + 100, &cpy, 100, NULL) == 5);
	T(tcsncmp(hello, cpy, 100) == 0);
	allocator_free(NULL, cpy);

	T(text_copy_new(hello, hello + 100, NULL, 100, NULL) == 0);

	T(text_copy_new(NULL, hello + 100, &cpy, 100, NULL) == 0);
	T(cpy == NULL);

	T(text_copy_new(hello + 3, hello, &cpy, 100, NULL) == 0);
	T(cpy == NULL);

	T(text_copy_new(hello, hello + 100, &cpy, 0, NULL) == 0);
	T(tcsncmp(e, cpy, 100) == 0);
	allocator_free(NULL, cpy);

	T(text_copy_new(hello, hello + 100, &cpy, 4, NULL) == 4);
	T(tcsncmp(TEXT("hell"), cpy, 100) == 0);
	allocator_free(NULL, cpy);

	T(text_copy_new(hello, hello + 100, &cpy, 5, NULL) == 5);
	T(tcsncmp(hello, cpy, 100) == 0);
	allocator_free(NULL, cpy);

    return 0;
}
