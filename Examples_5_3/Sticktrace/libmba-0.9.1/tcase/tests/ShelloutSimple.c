#include "common.h"
#include "mba/cfg.h"
#include "mba/misc.h"
#include "mba/shellout.h"

int
ShelloutSimple(int verbose, struct cfg *cfg, char *args[])
{
	struct sho *sh;
	char buf[256];
	const unsigned char *pv[] = { "sh> " };
	int n, i;
	cfg = NULL;

	sh = sho_open("sh", "sh> ", 0);

	n = sprintf(buf, "%s\n", args[0]); /* add newline */
	writen(sh->ptym, buf, n);
	i = sho_expect(sh, pv, 1, buf, 256, 10);
	if (i == 1) {
		tcase_printf(verbose, "success\n");
	} else if (i == -1) {
		perror("timeout");
	} else if (i == 0) {
		tcase_printf(verbose, "EOF\n");
	}

	n = sprintf(buf, "exit $?\n");
	writen(sh->ptym, buf, n);

	sho_close(sh);

	return i == 1 ? 0 : -1;
}
