#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <mba/diff.h>
#include <mba/hexdump.h>
#include <mba/msgno.h>

/* bindiff format is as follows:
 *
 * 	struct bdiff_patch {
 * 		struct bdiff_header {
 * 			uint16_t magic = 0xBD1F;
 * 			uint16_t version;
 * 			uint32_t num_edits;
 * 		};
 * 		struct {
 * 			unsigned short op;
 * 			unsigned short hash;
 * 			uint32_t off;
 * 			uint32_t len;
 * 		} edits[num_edits];
 * 		uint8_t payload[sum of DIFF_INSERT lengths];
 * 	};
 */

#if defined(__sparc__)
  #include <sys/inttypes.h>
#else
  #include <stdint.h>
#endif

/* encode/decode integers
 */

size_t
enc_uint16be(uint16_t s, unsigned char *dst)
{
	dst[0] = (s >> 8) & 0xFF;
	dst[1] = s & 0xFF;
	return 2;
}
size_t
enc_uint32be(uint32_t i, unsigned char *dst)
{
	dst[0] = (i >> 24) & 0xFF;
	dst[1] = (i >> 16) & 0xFF;
	dst[2] = (i >> 8) & 0xFF;
	dst[3] = i & 0xFF;
	return 4;
}
uint16_t
dec_uint16be(const unsigned char *src)
{
	return ((unsigned)src[0] << 8) | src[1];
}
uint32_t
dec_uint32be(const unsigned char *src)
{
	return ((unsigned)src[0] << 24) | ((unsigned)src[1] << 16) |
           ((unsigned)src[2] << 8) | src[3];
}

/* mmap an entire file into memory
 */

void *
mapfile(const char *filename, int *size)
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
int
do_patch(unsigned char *p, int pn, unsigned char *a, int n, FILE *stream)
{
	unsigned char *start = p;
	uint32_t hdr;
	int sn, i;

	if (pn < 8) {
		PMSG("invalid patch file");
		return -1;
	}

	/* decode header
	 */

	hdr = dec_uint32be(p); p += 4;
	if ((hdr & 0xFFFF0000) != 0xBD1F0000) {
		PMSG("invalid bindiff patch file header: 0x%8x", hdr);
		return -1;
	} else if ((hdr & 0xFFFF) > 0x0001) {
		PMSG("incompatible bindiff patch file version");
		return -1;
	}
	sn = dec_uint32be(p); p += 4;

	if (pn < (8 + sn * 12)) {
		PMSG("bindiff patch file corrupted");
		return -1;
	}

	/* decode/write edits
	 */

	for (i = 0; i < sn; i++) {
		int op, off, len;

		op = dec_uint16be(p); p += 4;
		off = dec_uint32be(p); p += 4;
		len = dec_uint32be(p); p += 4;

		switch (op) {
			case DIFF_MATCH:
				if ((off + len) > n) {
					PMSG("file being patched is truncated or patch file corrupted");
					return -1;
				}
				fwrite(a + off, 1, len, stream);
				break;
			case DIFF_INSERT:
				if ((off + len) > pn) {
					PMSG("patch file is truncated or corrupted");
					return -1;
				}
				fwrite(start + off, 1, len, stream);
				break;
		}
	}

	return 0;
}
int
do_diff(unsigned char *a, int n, unsigned char *b, int m, FILE *stream)
{
	int d;
	int sn, i;
	struct varray *ses = varray_new(sizeof(struct diff_edit), NULL);
	unsigned char buf[12];
	size_t off;

	if ((d = diff(a, 0, n, b, 0, m, NULL, NULL, NULL, 0, ses, &sn, NULL)) == -1) {
		MMNO(errno);
		return EXIT_FAILURE;
	}

	/* encode/write header
	 */

	enc_uint32be(0xBD1F0001, buf); /* magic and version */
	enc_uint32be(sn, buf + 4);             /* num_edits */

	if (fwrite(buf, 8, 1, stream) != 1) {
		PMNO(errno);
		return -1;
	}

	/* encode/write edits
	 */

	off = 8 + sn * 12;

	for (i = 0; i < sn; i++) {
		struct diff_edit *e = varray_get(ses, i);

		enc_uint16be(e->op, buf);
		enc_uint16be(0xab, buf + 2);

		switch (e->op) {
			case DIFF_MATCH:
				enc_uint32be(e->off, buf + 4);
				break;
			case DIFF_INSERT:
				enc_uint32be(off, buf + 4);
				off += e->len;
				break;
		}

		enc_uint32be(e->len, buf + 8);

		if (fwrite(buf, 12, 1, stream) != 1) {
			PMNO(errno);
			return -1;
		}
	}

	/* write payload
	 */

	for (i = 0; i < sn; i++) {
		struct diff_edit *e = varray_get(ses, i);

		if (e->op == DIFF_INSERT) {
			size_t n, len = e->len;

			while ((n = fwrite(b + e->off, 1, len, stream)) < len) {
				if (ferror(stream)) {
					PMNO(errno);
					return -1;
				}
				len -= n;
			}
		}
	}

	return d;
}
int
run(const char *file1, const char *file2, int patch)
{
	unsigned char *a, *b;
	int n, m;

	if ((a = mapfile(file1, &n)) == NULL || (b = mapfile(file2, &m)) == NULL) {
		AMSG("");
		return -1;
	}

	if (patch) {
		if (do_patch(a, n, b, m, stdout) == -1) {
			AMSG("failed to patch %s with %s", file2, file1);
			return -1;
		}
	} else if (do_diff(a, n, b, m, stdout) == -1) {
		AMSG("failed to diff %s with %s", file1, file2);
		return -1;
	}
	fflush(stdout);

	return 0;
}

int
main(int argc, char *argv[])
{
	char **args;
	char *file1 = NULL, *file2 = NULL;
	int patch = 0;

	if (argc < 3) {
usage:
		fprintf(stderr, "usage: %s [-p <patchfile>] <file1> [<file2>]\n", argv[0]);
		return EXIT_FAILURE;
	}

	errno = 0;

	args = argv;
	args++; argc--;

	while (argc) {
		if (!patch && strcmp(*args, "-p") == 0) {
			patch = 1;
			args++; argc--;
			if (argc == 0) {
				fprintf(stderr, "-p requires a patch filename argument\n");
				goto usage;
			}
			file1 = *args;
		} else if (file2) {
			fprintf(stderr, "invalid arguments\n");
			goto usage;
		} else if (file1) {
			file2 = *args;
		} else {
			file1 = *args;
		}

		args++; argc--;
	}

	if (file1 == NULL || file2 == NULL) {
		goto usage;
	}

	if (run(file1, file2, patch) == -1) {
		MMSG("");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


