#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>

#include <mba/hashmap.h>
#include <mba/csv.h>
#include <mba/msgno.h>

#define INMAX 32

struct ent {
	char ident[32];
	char value[32];
	char desc[512];
};

int
load_map(struct hashmap *map, char *fname)
{
	FILE *in;
	unsigned char buf[8192];
	unsigned char *row[3];
	int n, i = 0;
	struct ent *ents;

	if ((in = fopen(fname, "r")) == NULL) {
		PMNF(errno, ": %s", fname);
		return -1;
	}

	ents = malloc(255 * sizeof *ents);

	while ((n = csv_row_fread(in, buf, 8192, row, 3, '\t', CSV_TRIM | CSV_QUOTES)) > 0) {
		struct ent *ent = ents + i++;
		if (!row[0] || strcmp(row[0], "") == 0) {
			PMSG("Identifer cannot be NULL or blank: %s:%d", fname, i);
			return -1;
		}
		strcpy(ent->ident, row[0]);
		strcpy(ent->value, row[1] ? row[1] : (unsigned char *)"");
		strcpy(ent->desc, row[2] ? row[2] : (unsigned char *)"");
		if (hashmap_put(map, ent->ident, ent) == -1) {
			AMSG("");
			return -1;
		}
	}
	fclose(in);
	if (n == -1) {
		AMSG("");
		return -1;
	}

	return 0;
}

int
run(int incount, char *in[])
{
	struct hashmap maps[INMAX];
	struct ent *line[INMAX];
	iter_t iter;
	char *ident;
	int p, i;

	for (i = 0; i < incount; i++) {
		hashmap_init(&maps[i], 0, hash_str, cmp_str, NULL, NULL);
		if (load_map(&maps[i], in[i]) == -1) {
			AMSG("");
			return -1;
		}
	}

/* For each systems' errno map iterate over each key and use it to remove
 * any matches in the other maps placing the entries in the corresponding
 * index of the line array and print the line. Then move to the next map
 * and repeat. With each iteration of the outer loop the number of keys in
 * the remaining maps should decrease until only errno identifiers unique
 * to the last map remain.
 */
	for (p = 0; p < incount; p++) {
		hashmap_iterate(&maps[p], &iter);
		while ((ident = hashmap_next(&maps[p], &iter))) {
			float portability;
			char *id;

			memset(line, 0, sizeof(line));
			line[p] = hashmap_get(&maps[p], ident);
                   /* Go through all maps after p and remove matches
                    */
			for (i = (p + 1); i < incount; i++) {
				char *key = ident;
				struct ent *ent;

				if (hashmap_remove(&maps[i], (void **)&key, (void **)&ent) == 0) {
					line[i] = ent;
				} else {
					line[i] = NULL;
				}
			}
                   /* Now go through each ent in the line and
                    * print the tuple
                    */
			portability = 0.0;
			for (i = 0; i < incount; i++) {
				if (line[i]) {
					portability += 1.0;
					id = line[i]->ident;
				}
			}
			fprintf(stdout, "%s\t%.2f\t", id, portability / incount);
			for (i = 0; i < incount; i++) {
				if (line[i]) {
					fprintf(stdout, "%s\t%s\t", line[i]->value, line[i]->desc);
				} else {
					fprintf(stdout, "\t\t");
				}
			}
			fprintf(stdout, "\n");
		}
	}

	return 0;
}
int
main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s <file1> [<file2 [...]]\n", argv[0]);
		return EXIT_FAILURE;
	}
	if (run(argc - 1, argv + 1) == -1) {
		MMSG("");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

