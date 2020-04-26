#ifndef MBA_CSV_H
#define MBA_CSV_H

/* csv - read write comma separated value format
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LIBMBA_API
#ifdef WIN32
# ifdef LIBMBA_EXPORTS
#  define LIBMBA_API  __declspec(dllexport)
# else /* LIBMBA_EXPORTS */
#  define LIBMBA_API  __declspec(dllimport)
# endif /* LIBMBA_EXPORTS */
#else /* WIN32 */
# define LIBMBA_API extern
#endif /* WIN32 */
#endif /* LIBMBA_API */

#include <stdio.h>
#include <mba/text.h>

#if USE_WCHAR
#define csv_row_parse csv_row_parse_wcs
#else
#define csv_row_parse csv_row_parse_str
#endif

#define CSV_TRIM   0x01
#define CSV_QUOTES 0x02

LIBMBA_API int csv_row_parse_str(const unsigned char *src, size_t sn,
		unsigned char *buf, size_t bn,
		unsigned char *row[], int rn,
		int sep, int flags);
LIBMBA_API int csv_row_parse_wcs(const wchar_t *src, size_t sn,
		wchar_t *buf, size_t bn,
		wchar_t *row[], int rn,
		int sep, int flags);
LIBMBA_API int csv_row_fread(FILE *in,
		unsigned char *buf, size_t bn,
		unsigned char *row[], int numcols,
		int sep, int flags);

#ifdef __cplusplus
}
#endif

#endif /* MBA_CSV_H */
