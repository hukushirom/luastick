#include "common.h"
#include "mba/cfg.h"
#include "mba/domnode.h"

int
DomnodeLoadStore(int verbose, struct cfg *cfg, char *args[])
{
	struct domnode *root, *root2;
	cfg = NULL;

	root = domnode_new(NULL, NULL, NULL);
	if (domnode_load(root, args[0]) == -1) {
		AMSG("Failed to load XML file: %s", args[0]);
		return -1;
	}
	if (domnode_store(root, args[1]) == -1) {
		AMSG("Failed to store XML file: %s", args[1]);
		return -1;
	}
	domnode_del(root);
	/* load the output to santity check it
	 */
	root2 = domnode_new(NULL, NULL, NULL);
	if (domnode_load(root2, args[1]) == -1) {
		AMSG("Failed to load XML file: %s", args[1]);
		return -1;
	}
	if (domnode_store(root2, args[2]) == -1) {
		AMSG("Failed to store XML file: %s", args[2]);
		return -1;
	}
	domnode_del(root2);

	tcase_printf(verbose, "done");

    return 0;
}
