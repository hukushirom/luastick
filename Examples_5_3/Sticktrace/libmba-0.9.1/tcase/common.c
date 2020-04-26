#include "common.h"
#if !defined(_WIN32)
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

volatile sig_atomic_t sig;

const char *signal_names[] = {
	"", "SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP", "SIGABRT", "SIGBUS", "SIGFPE", "SIGKILL", "SIGUSR1", "SIGSEGV", "SIGUSR2", "SIGPIPE", "SIGALRM", "SIGTERM", "SIGSTKFLT", "SIGCHLD", "SIGCONT", "SIGSTOP", "SIGTSTP", "SIGTTIN", "SIGTTOU", "SIGURG", "SIGXCPU", "SIGXFSZ", "SIGVTALRM", "SIGPROF", "SIGWINCH", "SIGPOLL/SIGIO", "SIGPWR", "SIGSYS" };

const char *
signalstr(int sig)
{
	if (sig < 1 || sig > 31) {
		return "SIGUNKNOWN";
	}
	return signal_names[sig];
}
void
sig_handler(int s)
{
	sig = s;
	fprintf(stderr, "%s\n", signalstr(s));
	if (s == SIGSEGV || s == SIGINT) {
		raise(SIGKILL);
	}
	sleep(1);
}
int
set_signals(void)
{
	struct sigaction sact;
	int sig_list[] = { SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGABRT, SIGFPE,
					   SIGSEGV, SIGPIPE, SIGTERM, SIGBUS, 0 };
	int i;

	for (i = 0; sig_list[i] != 0; i++) {
		sact.sa_handler = sig_handler;
		sigemptyset(&sact.sa_mask);
		sact.sa_flags = 0;
		if (sigaction(sig_list[i], &sact, NULL) < 0) {
			return -1;
		}
	}

	return 0;
}
int
randint(int mn, int mx)
{
	float ran = rand() / RAND_MAX;
	float f = (mx - mn) * ran;
	return mn + f;
}

void *
open_mmap(const char *name, int flags, int mode, size_t size)
{
	char *fname, buf[32] = "/tmp/";
	int i, fd, prot = PROT_READ;
	void *ret;

	fname = buf + 5;
	for (i = 0; i < 22 && name[i]; i++) {
		if (name[i] == '.' || name[i] == '/') {
			errno = EINVAL;
			PMNO(errno);
			return NULL;
		}
		fname[i] = name[i];
	}
	strcpy(fname + i, ".shm");

	if ((fd = open(buf, flags, mode)) == -1 || (mode && fchmod(fd, mode) == -1)) {
		PMNF(errno, ": %s", buf);
		return NULL;
	}
	if (ftruncate(fd, size) == -1) {
		close(i);
		PMNO(errno);
		return NULL;
	}

	if ((flags & O_RDWR)) {
		prot |= PROT_WRITE;
	}
	if ((ret = mmap(NULL, size, prot, MAP_SHARED, fd, 0)) == MAP_FAILED) {
		PMNO(errno);
		return NULL;
	}
	close(fd);

	return ret;
}
#endif

void
rate_iterate(iter_t *iter)
{
	iter->i1 = 0;
}
int
rate_next(iter_t *iter)
{
	if (iter->i1) {
		iter->i1 = 0;
		return (int)randint(0,3);
	} else {
		iter->i1 = 1;
		return (int)randint(5,9);
	}
}
