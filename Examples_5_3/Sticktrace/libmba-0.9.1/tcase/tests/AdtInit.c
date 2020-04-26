#include "common.h"
#include "mba/cfg.h"
#include "mba/suba.h"
#include "mba/stack.h"
#include "mba/linkedlist.h"
#include "mba/varray.h"

int
AdtInit(int verbose, struct cfg *cfg, char *args[])
{
	char buf[0xFFFF];
	struct allocator *suba;
	struct stack s;
	struct linkedlist l;
	struct varray va;

	if ((suba = suba_init(buf, 0xFFFF, 1, 0)) == NULL ||
			stack_init(&s, 0, suba) == -1 ||
			linkedlist_init(&l, 0, suba) == -1 ||
			varray_init(&va, sizeof(int), suba) == -1) {
		AMSG("");
		return -1;
	}

	linkedlist_add(&l, "two");
	stack_push(&s, "two");
	linkedlist_add(&l, "three");
	stack_push(&s, "one");
	varray_get(&va, 444);
	stack_push(&s, "three");
	varray_get(&va, 4);
	varray_get(&va, 44);
	linkedlist_add(&l, "one");

	if (varray_deinit(&va) != 0 ||
			linkedlist_deinit(&l, NULL, NULL) != 0 ||
			stack_deinit(&s, NULL, NULL) != 0) {
		AMSG("");
		return -1;
	}

	tcase_printf(verbose, "done ");

	cfg = NULL;
	args[0] = NULL;
    return 0;
}
