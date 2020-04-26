#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <mba/hashmap.h>
#include <mba/eval.h>

int
symtab_lookup(const unsigned char *key, unsigned long *val, struct hashmap *symtab)
{
	const unsigned char *str;

	if ((str = hashmap_get(symtab, key))) {
		*val = strtoul(str, NULL, 0);
		return 0;
	}

	return -1;
}
int
main(void)
{
	struct hashmap symtab;
	unsigned char buf[1024], *bp = buf;
	unsigned long result;
	struct eval *eval = eval_new((symlook_fn)symtab_lookup, &symtab);

	hashmap_init(&symtab, 0, hash_str, cmp_str, NULL, NULL);

	while ((*bp = fgetc(stdin)) > 0) {
		if (*bp == '\n') {
			*bp = '\0';
			bp = strchr(buf, '=');
			*bp++ = '\0';
			eval_expression(eval, bp, bp + strlen(bp), &result);
			sprintf(bp, "%lu", result);
			printf(" %s=%ld\n", buf, result);
			hashmap_put(&symtab, strdup(buf), strdup(bp));
			bp = buf;
		} else {
			bp++;
		}
	}

	return EXIT_SUCCESS;
}
