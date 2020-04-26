#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mba/suba.h>
#include <mba/diff.h>
#include <mba/msgno.h>

/* usage:
 * $ ./spell skoke
 * poke
 * spoke
 * spoken
 * 
 * In practice you really need scoring so adding -ing or switching ei with
 * ie costs less than two otherwise unrelated characters.
 */

#define DIFF_LIMIT 4

char *words[] = {
	"poke",
	"poker",
	"pokeweed",
	"recessed",
	"recession",
	"recessional",
	"recessionary",
	"recessive",
	"recharge",
	"rechargeable",
	"rechauffe",
	"recidivism",
	"recidivist",
	"Recife",
	"recipe",
	"recipient",
	"reciprocal",
	"reciprocality",
	"reciprocate",
	"reciprocating",
	"reciprocation",
	"reciprocative",
	"reciprocity",
	"recission",
	"recital",
	"recitalist",
	"recitation",
	"recitative",
	"recite",
	"reciter",
	"recklessly",
	"recklessness",
	"reckon",
	"reckoner",
	"reckoning",
	"reclaim",
	"reclaimable",
	"reclamation",
	"reclassification",
	"reclassify",
	"recline",
	"recliner",
	"reclining",
	"recluse",
	"split",
	"splitter",
	"splotch",
	"splurge",
	"splutter",
	"spluttering",
	"Spock",
	"Spode",
	"Spodoptera",
	"spodumene",
	"spoil",
	"spoilage",
	"spoiled",
	"spoiler",
	"spoilsport",
	"Spokane",
	"spoke",
	"spoken",
	"spokesman",
	"spokesperson",
	"spokeswoman",
	NULL
};
int
lookup(const char *word)
{
	unsigned char mem[4096];
	struct allocator *al = suba_init(mem, 4096, 1, 0);
	struct varray buf;
	int m[1024], max, i, wlen = strlen(word), d;

	varray_init(&buf, sizeof(int), al);

	for (i = 0; words[i]; i++) {
		m[i] = strlen(words[i]);
	}
	max = i;

	for (i = 0; i < max; i++) {
		if ((d = diff(word, 0, wlen, words[i], 0, m[i],
					NULL, NULL, NULL, DIFF_LIMIT, NULL, NULL, &buf)) == -1) {
			MMNO(errno);
			return EXIT_FAILURE;
		}
		if (d == 0) {
			printf("exact match\n");
			return 0;
		} else if (d < DIFF_LIMIT) {
			printf("%s\n", words[i]);
		}
	}

	return 0;
}

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "usage: spell <word>\n");
		return EXIT_FAILURE;
	}

	if (lookup(argv[1]) == -1) {
		MMSG("");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

