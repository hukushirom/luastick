#include "common.h"
#include "mba/cfg.h"
#include "mba/bitset.h"
#include "mba/hexdump.h"

/*
0000 0000 0011 1111 1111 2222 2222 2233
0123 4567 8901 2345 6789 0123 4567 8901
*/

int
BitsetOps(int verbose, struct cfg *cfg, char *args[])
{
	unsigned char bitset[] = {
		0x00, 0xFF, 0xFF, 0x8F, /*  0 - 31 */
		0xF0, 0xF0, 0xF0, 0x80, /* 32 - 63 */
		0x34, 0x12, 0xCD, 0xAB  /* 64 - 95 */
	};
	iter_t iter;
	int val;

	if (bitset_isset(bitset, 1) != 0 ||
			bitset_isset(bitset, 8) != 1 ||
			bitset_isset(bitset, 30) != 0 ||
			bitset_isset(bitset, 40) != 0 ||
			bitset_isset(bitset, 63) != 1 ||
			bitset_isset(bitset, 64) != 0 ||
			bitset_isset(bitset, 94) != 0 ||
			bitset_isset(bitset, 95) != 1) {
		MMSG("");
		return -1;
	}

	bitset_set(bitset, 30);
	if (bitset_isset(bitset, 30) == 0) {
		MMSG("");
		return -1;
	}
	bitset_unset(bitset, 30);
	if (bitset_isset(bitset, 30) == 1) {
		MMSG("");
		return -1;
	}
	bitset_toggle(bitset, 30);
	if (bitset_isset(bitset, 30) == 0) {
		MMSG("");
		return -1;
	}

	if (bitset_find_first(bitset, bitset + 12, 1) != 8 ||
			bitset_find_first(bitset + 1, bitset + 11, 0) != 20) {
		MMSG("");
		return -1;
	}

	bitset_iterate(&iter);
	while ((val = bitset_next(bitset, bitset + 12, &iter)) != -1) {
		if (verbose) {
			fputc('0' + val, stderr);
			if ((iter.i1 % 8) == 0) {
				fputc(' ', stderr);
			}
		}
	}
	if (verbose) {
		fputc('\n', stderr);
		hexdump(stderr, bitset, 12, 12);
	}

	tcase_printf(verbose, "done\n");
	cfg = NULL;
	args = NULL;

    return 0;
}
