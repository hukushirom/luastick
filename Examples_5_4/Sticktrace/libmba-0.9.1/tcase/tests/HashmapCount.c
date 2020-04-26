#include "common.h"
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "mba/suba.h"
#include "mba/hashmap.h"

struct test {
	char *src;
	char *slim;
	struct hashmap *words;
	struct allocator *al;
};

int
get_word(struct test *t, char **w)
{
	int count = 0;

	for ( ; t->src < t->slim; t->src++) {
		if (isspace(*t->src)) {
			if (count) {
				*t->src++ = '\0';
				return count;
			}
		} else {
			if (!count) {
				*w = t->src;
			}
			count++;
		}
	}

	return 0; /* EOF */
}
int
load_words(struct test *t)
{
	int n;
	char *word;
	int count = 1;

	while ((n = get_word(t, &word)) > 0) {
errno = 0;
		if (hashmap_put(t->words, word, (void *)count) == -1) {
			char *key = word;
			int data;

			if (errno != EEXIST) {
				AMSG("");
				return -1;
			}
			if (hashmap_remove(t->words, (void **)&key, (void **)&data) == -1) {
				AMSG("");
				return -1;
			}
			data++;
			if (hashmap_put(t->words, word, (void *)data) == -1) {
				AMSG("");
				return -1;
			}
		}
	}
	if (n == -1) {
		MMSG("");
		return -1;
	}

	return 0;
}

int
print_key(void *context, void *object)
{
	fprintf(stderr, "%s\n", (char *)object);
	context = NULL;
	return 0;
}
int
print_values(struct hashmap *words)
{
	iter_t iter;
	char *word;
	int val;

	hashmap_iterate(words, &iter);
	while ((word = hashmap_next(words, &iter))) {
		val = (int)hashmap_get(words, word);
		fprintf(stderr, "%d %s\n", val, word);
	}

	return 0;
}

int
HashmapCount(int verbose, struct cfg *cfg, char *args[])
{
	struct test t;
	char *mem = NULL;
	int ret = 0, fd = 0, data;
	int useal = atoi(args[0]);
	struct stat st;
	void *mm;

	if (useal) {
		if ((mem = malloc(0xFFFFFF)) == NULL ||
				(t.al = suba_init(mem, 0xFFFFFF, 1, 0)) == NULL) {
			AMSG("");
			ret = -1;
			goto err;
		}
	} else {
		t.al = NULL;
	}
	if ((t.words = hashmap_new(hash_str, (cmp_fn)strcmp, NULL, t.al)) == NULL ||
				(fd = open(args[1], 0)) == -1 ||
				fstat(fd, &st) == -1) {
		AMSG("");
		ret = -1;
		goto err;
	}
	if ((t.src = mm = mmap(NULL, (int)st.st_size,
					PROT_READ | PROT_WRITE,
					MAP_PRIVATE,
					fd, 0)) == NULL) {
		AMSG("");
		ret = -1;
		goto err;
	}

	t.slim = t.src + st.st_size;
	if (load_words(&t) == -1) {
		AMSG("");
		ret = -1;
		goto err;
	}

	if ((data = (int)hashmap_get(t.words, args[2])) == 0 ||
			atoi(args[3]) != data ||
			(data = (int)hashmap_get(t.words, args[4])) == 0 ||
			atoi(args[5]) != data) {
		errno = ERANGE;
		PMNF(errno, ": %d != %d", atoi(args[3]), data);
		ret = -1;
		goto err;
	}

if (verbose)
	print_values(t.words);
err:
	munmap(mm, st.st_size);
	if (fd)
		close(fd);
	hashmap_del(t.words, NULL, NULL, NULL);
	free(mem);

	cfg = NULL;
	return ret;
}

