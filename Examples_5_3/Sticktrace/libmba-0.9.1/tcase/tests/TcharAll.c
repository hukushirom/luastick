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
TcharAll(int verbose, struct cfg *cfg, char *args[])
{
	tchar buf[1024];
	tchar *hello = TEXT("hello");
	tchar *str;
	double dbl;
	float flt;
	long double ldbl;
	long l;
	unsigned long ul;
#if defined(__USE_BSD)
	unsigned long long ull;
	long long ll;
#endif
	time_t t;
	FILE *nowhere = tmpfile();
	cfg = NULL; *args = NULL; verbose = 1; ldbl = 0;

	/* int istalnum(tint_t tc);
	 */
	T(istalnum(TEXT('4')));
	/* int istalpha(tint_t tc);
	 */
	T(istalpha(TEXT('A')));
	/* int istcntrl(tint_t tc);
	 */
	T(istcntrl(0x16));
	/* int istdigit(tint_t tc);
	 */
	T(istdigit(TEXT('4')));
	/* int istgraph(tint_t tc);
	 */
	T(istgraph(TEXT('A')));
	/* int istlower(tint_t tc);
	 */
	T(istlower(TEXT('a')));
	/* int istprint(tint_t tc);
	 */
	T(istprint(TEXT('A')));
	/* int istpunct(tint_t tc);
	 */
	T(istpunct(TEXT('.')));
	/* int istspace(tint_t tc);
	 */
	T(istspace(TEXT('	')));
	/* int istupper(tint_t tc);
	 */
	T(istupper(TEXT('A')));
	/* int istxdigit(tint_t tc);
	 */
	T(istxdigit(TEXT('f')));
#if (_XOPEN_VERSION >= 500)
	/* int istblank(tint_t tc);
	 */
	T(istblank(TEXT('	')));
#endif
	/* tint_t totlower(tint_t tc);
	 */
	T(totlower(TEXT('A')) == TEXT('a'));
	/* tint_t totupper(tint_t tc);
	 */
	T(totupper(TEXT('a')) == TEXT('A'));
	/* tchar *tcscpy(tchar *dest, const tchar *src);
	 */
	T(tcscmp(tcscpy(buf, hello), hello) == 0);
	/* tchar *tcsncpy(tchar *dest, const tchar *src, size_t n);
	 */
	T(tcscmp(tcsncpy(buf, hello, 1024), hello) == 0);
	/* tchar *tcscat(tchar *dest, const tchar *src);
	 */
	T(tcscmp(tcscat(buf, TEXT(", world")), TEXT("hello, world")) == 0);
	/* tchar *tcsncat(tchar *dest, const tchar *src, size_t n);
	 */
	T(tcscmp(tcsncat(buf, TEXT(", again"), 1024), TEXT("hello, world, again")) == 0);
	/* int tcscmp(const tchar *s1, const tchar *s2);
	 */
	T(tcscmp(TEXT("alpha"), TEXT("beta")) < 0);
	/* int tcsncmp(const tchar *s1, const tchar *s2, size_t n);
	 */
	T(tcsncmp(TEXT("beta"), TEXT("alpha"), 1024) > 0);
	/* int tcscoll(const tchar *s1, const tchar *s2);
	 */
	T(tcscoll(TEXT("alpha"), TEXT("beta")) < 0);
	/* size_t tcsxfrm(tchar *s1, const tchar *s2, size_t n);
         what should the result be?
	T(tcsxfrm(buf, hello, 1024) == 5);
	 */
/* How do you get a locale_t?
 */
	/* size_t tcsxfrm_l(tchar *s1, const tchar *s2, size_t n, locale_t loc);
	 * int tcscoll_l(const tchar *s1, const tchar *s2, locale_t loc);
	 * int tcscasecmp_l(const tchar *s1, const tchar *s2, locale_t loc);
	 * int tcsncasecmp_l(const tchar *s1, const tchar *s2, size_t n, locale_t loc);
	 * long int tcstol_l(const tchar *nptr, tchar **endptr, int base, locale_t loc);
	 * unsigned long int tcstoul_l(const tchar *nptr, tchar **endptr, int base, locale_t loc);
	 * long long int tcstoll_l(const tchar *nptr, tchar **endptr, int base, locale_t loc);
	 * unsigned long long int tcstoull_l(const tchar *nptr, tchar **endptr, int base, locale_t loc);
	 * double tcstod_l(const tchar *nptr, tchar **endptr, locale_t loc);
	 * float tcstof_l(const tchar *nptr, tchar **endptr, locale_t loc);
	 * long double tcstold_l(const tchar *nptr, tchar **endptr, locale_t loc);
	 */
	/* tchar *tcsdup(const tchar *s);
	 */
	T(tcscmp(hello, (str = tcsdup(hello))) == 0);
	free(str);
	/* tchar *tcschr(const tchar *tcs, tchar tc);
	 */
	T((tchar *)tcschr(hello, TEXT('l')) == (hello + 2));
	/* tchar *tcsrchr(const tchar *tcs, tchar tc);
	 */
	T((tchar *)tcsrchr(hello, TEXT('l')) == (hello + 3));
	/* size_t tcscspn(const tchar *tcs, const tchar *reject);
	 */
	T(tcscspn(hello, TEXT("abo")) == 4);
	/* size_t tcsspn(const tchar *tcs, const tchar *accept);
	 */
	T(tcsspn(hello, TEXT("hel")) == 4);
	/* tchar *tcspbrk(const tchar *tcs, const tchar *accept);
	 */
	T((tchar *)tcspbrk(hello, TEXT("abo")) == (hello + 4));
	/* tchar *tcsstr(const tchar *haystack, const tchar *needle);
	 */
	T((tchar *)tcsstr(hello, TEXT("ell")) == (hello + 1));
	/* tchar *tcstok(tchar *s, const tchar *delim, tchar **ptr);
	 */
	tcscpy(buf, hello);
	T((tchar *)tcstok(buf, TEXT("l"), (char **)&str) == buf);
	T((tchar *)tcstok(NULL, TEXT("l"), (char **)&str) == (buf + 4));
	/* size_t tcslen(const tchar *s);
	 */
	T(tcslen(hello) == 5);
#if !defined(_WIN32)
	/* tchar *tmemcpy(tchar *s1, const tchar *s2, size_t n);
	 */
	T(tmemcpy(buf, hello, 3) == buf);
	/* tchar *tmemmove(tchar *s1, const tchar *s2, size_t n);
	 */
	T(tmemmove(buf + 5, buf, 5) == (buf + 5));
	/* int tmemcmp(const tchar *s1, const tchar *s2, size_t n);
	 */
	T(tmemcmp(buf + 5, hello, 5) == 0);
	/* tchar *tmemset(tchar *s, tchar c, size_t n);
	 */
	T(tmemset(buf, TEXT('x'), 1024) == buf);
	/* tchar *tmemchr(const tchar *s, tchar c, size_t n);
	 */
	T(tmemchr(hello, TEXT('l'), 3) == (hello + 2));
#endif
	/* tchar *tcpncpy(tchar *dest, const tchar *src, size_t n);
	 */
/* This writes 'helo' into buf. It should be 'hello'.
	tcpcpy(tcpncpy(buf, TEXT("hello"), 3), TEXT("lo"));
	T(tcscmp(buf, hello) == 0);
*/
	/* double tcstod(const tchar *nptr, tchar **endptr);
	 */
	T((dbl = tcstod(TEXT("1.56"), NULL)) > 1.55 && dbl < 1.57);
#if (__STDC_VERSION__ >= 199901L)
	/* float tcstof(const tchar *nptr, tchar **endptr);
	 */
	T((flt = tcstof(TEXT("1.56"), NULL)) > 1.55 && flt < 1.57);
	/* long double tcstold(const tchar *nptr, tchar **endptr);
	 */
	T((ldbl = tcstold(TEXT("1.56"), NULL)) > 1.55 && ldbl < 1.57);
#endif
	/* long int tcstol(const tchar *nptr, tchar **endptr, int base);
	 */
	T((l = tcstol(TEXT("101"), NULL, 10)) == 101);
	/* unsigned long int tcstoul(const tchar *nptr, tchar **endptr, int base)
	 */
	T((ul = tcstoul(TEXT("101"), NULL, 10)) == 101);
	/* int ftprintf(FILE *stream, const tchar *format, ...);
	 */
#ifdef WIDE_IO
#ifdef USE_WCHAR
	T(ftprintf(nowhere, TEXT("%ls"), hello) == 5);
#else
	T(ftprintf(nowhere, TEXT("%s"), hello) == 5);
#endif
	/* int tprintf(const tchar *format, ...);
	 */
#ifdef USE_WCHAR
	T(tprintf(TEXT("%ls"), hello) == 5);
#else
	T(tprintf(TEXT("%s"), hello) == 5);
#endif
	/* int vftprintf(FILE *s, const tchar *format, va_list arg);
	 */
	if (0) {
		vftprintf(nowhere, TEXT("%s"), hello);
	}
	/* int vtprintf(const tchar *format, va_list arg);
	 */
	if (0) {
		vtprintf(TEXT("%s"), hello);
	}
	/* int ftscanf(FILE *stream, const tchar *format, ...);
	 */
	if (0) {
		ftscanf(stdin, TEXT("%s"), hello);
	}
	/* int tscanf(const tchar *format, ...);
	 */
	if (0) {
		tscanf(TEXT("%s"), hello);
	}
	/* int vftscanf(FILE *s, const tchar *format, va_list arg);
	 */
	if (0) {
		vftscanf(stdin, TEXT("%s"), hello);
	}
	/* int vtscanf(const tchar *format, va_list arg);
	 */
	if (0) {
		vtscanf(TEXT("%s"), hello);
	}
	/* tint_t fgettc(FILE *stream);
	 */
	if (0) {
		fgettc(stdin);
	}
	/* tint_t gettc(FILE *stream);
	 */
	if (0) {
		gettc(stdin);
	}
	/* tint_t gettchar(void);
	 */
	if (0) {
		gettchar();
	}
	/* tint_t gettc_unlocked(FILE *stream);
	 */
	if (0) {
		gettc_unlocked(stdin);
	}
	/* tint_t gettchar_unlocked(void);
	 */
	if (0) {
		gettchar_unlocked();
	}
	/* tint_t fgettc_unlocked(FILE *stream);
	 */
	if (0) {
		fgettc_unlocked(stdin);
	}
	/* tint_t fputtc(tchar tc, FILE *stream);
	 */
	T(fputtc(TEXT('a'), nowhere) == TEXT('a'));
	/* tint_t puttc(tchar tc, FILE *stream);
	 */
	T(puttc(TEXT('a'), nowhere) == TEXT('a'));
	/* tint_t puttchar(tchar tc);
	 */
	if (0) {
		puttchar(TEXT('a'));
	}
	/* tint_t fputtc_unlocked(tchar tc, FILE *stream);
	 */
	T(fputtc_unlocked(TEXT('a'), nowhere) == TEXT('a'));
	/* tint_t puttc_unlocked(tchar tc, FILE *stream);
	 */
	T(puttc_unlocked(TEXT('a'), nowhere) == TEXT('a'));
	/* tint_t puttchar_unlocked(tchar tc);
	 */
	T(puttchar_unlocked(TEXT('a')) == TEXT('a'));
	/* tchar *fgetts(tchar *ts, int n, FILE *stream);
	 */
	if (0) {
		fgetts(buf, 1024, stdin);
	}
	/* tchar *fgetts_unlocked(tchar *ts, int n, FILE *stream);
	 */
	if (0) {
		fgetts_unlocked(buf, 1024, stdin);
	}
	/* int fputts_unlocked(const tchar *ts, FILE *stream);
	 */
	T(fputts_unlocked(hello, nowhere) != TEOF);
	/* tint_t ungettc(int tc, FILE *stream);
	 */
	T(ungettc(TEXT('a'), nowhere) != TEOF);
#endif /* WIDE_IO */

	/* int fputts(const tchar *ts, FILE *stream);
	 */
	T(fputts(hello, nowhere) != TEOF);
	/* int stprintf(tchar *s, size_t maxlen, const tchar *format, ...);
	 */
#ifdef USE_WCHAR
	T(stprintf(buf, 1024, TEXT("%ls"), hello) == 5);
/* INCONSISTANT; swprintf returns -1 */
	T(stprintf(buf, 4, hello) == -1);
#else
	T(stprintf(buf, 1024, TEXT("%s"), hello) == 5);
#if defined(_WIN32)
	T(stprintf(buf, 4, hello) == -1);
#elif (__STDC_VERSION__ >= 199901L)
	T(stprintf(buf, 4, hello) == 5);
#else
	T(stprintf(buf, 4, hello) == 3);
#endif
#endif
	/* int vstprintf(tchar *s, size_t n, const tchar *format, va_list arg);
	 */
	if (0) {
		va_list args;
		vstprintf(buf, 1024, TEXT("%s"), args);
	}
	/* int stscanf(const tchar *s, const tchar *format, ...);
	 */
	tcscpy(buf, TEXT("2.3"));
	T(stscanf(buf, TEXT("%f"), &flt) == 1);
#if defined(__USE_GNU)
	/* size_t tcsnlen(const tchar *s, size_t maxlen);
	 */
	T(tcsnlen(hello, 3) == 3);
	/* tchar *tcschrnul(const tchar *s, tchar tc);
	 */
	T((tchar *)tcschrnul(hello, TEXT('x')) == (hello + 5));
	/* tchar *tcpcpy(tchar *dest, const tchar *src);
	 */
	tcpcpy(tcpcpy(buf, TEXT("hel")), TEXT("lo"));
	/* int vstscanf(const tchar *s, const tchar *format, va_list arg);
	 */
	if (0) {
		vstscanf(buf, TEXT("%s"), hello);
	}
#else
	tcscpy(buf, "hello");
#endif /* __USE_GNU */
#if defined(__USE_BSD)
	/* int tcscasecmp(const tchar *s1, const tchar *s2);
	 */
	T(tcscasecmp(hello, TEXT("HELlO")) == 0);
	/* int tcsncasecmp(const tchar *s1, const tchar *s2, size_t n);
	 */
	T(tcsncasecmp(hello, TEXT("HELle"), 4) == 0);
	T(tcscmp(buf, hello) == 0);
	/* long long int tcstoq(const tchar *nptr, tchar **endptr, int base);
	 */
	T((ll = tcstoq(TEXT("101"), NULL, 10)) == 101);
	/* unsigned long long int tcstouq(const tchar *nptr, tchar **endptr, int base);
	 */
	T((ull = tcstouq(TEXT("101"), NULL, 10)) == 101);
	/* long long int tcstoll(const tchar *nptr, tchar **endptr, int base);
	 */
	T((ll = tcstoll(TEXT("101"), NULL, 10)) == 101);
	/* unsigned long long int tcstoull(const tchar *nptr, tchar **endptr, int base);
	 */
	T((ull = tcstoull(TEXT("101"), NULL, 10)) == 101);
#endif /* __USE_BSD */
	/* size_t tcsftime(tchar *s, size_t maxsize, const tchar *format, const struct tm *tp);
	 */
	time(&t);
	tcsftime(buf, 1024, TEXT("%m/%d/%Y"), localtime(&t));

    return 0;
}
