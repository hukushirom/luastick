#include "common.h"
#include "mba/cfg.h"
#include "mba/text.h"

int
CfgOps(int verbose, struct cfg *cfg, char *args[])
{
	tchar *instr = _T("a=z\nc=d\n\ne=f");
	int i;
	char buf[1024];

	cfg = cfg_new(NULL);
	if (cfg_load(cfg, args[0]) == -1) {
		AMSG("Failed to load prp file: %s", args[0]);
		return -1;
	}

	if (cfg_load_env(cfg) == -1 ||
			cfg_load_str(cfg, instr, instr + 100) == -1) {
		AMSG("");
		return -1;
	}
	i = 1;
	if (cfg_vget_int(cfg, &i, 500, _T("mail.folder.%d.idx"), i) == -1) {
		AMSG("");
		return -1;
	}
	if (i != 2) {
		PMSG("mail.folder.1.idx == %d", i);
		return -1;
	}

	if (cfg_get_str(cfg, buf, 1024, NULL, "fruits") == -1 ||
				strcmp(buf, "apple, banana, pear, cantaloupe, watermelon, kiwi, mango") != 0) {
		PMSG("buf=>%s<", buf);
		return -1;
	}
	if (cfg_get_str(cfg, buf, 1024, NULL, "foo") == -1 ||
				strcmp(buf, "b	ar ") != 0) {
		PMSG("buf=>%s<", buf);
		return -1;
	}

	cfg_del(cfg);
	tcase_printf(verbose, "done");

    return 0;
}
