/* Copyright (c) 2003 Michael B. Allen <mba2000 ioplex.com>
 *
 * The MIT License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <limits.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define FLAGS_SEMUNDO 1
#define IS_SEMUNDO(cmd) (((cmd)->flags & FLAGS_SEMUNDO) != 0)

#define MAX_TRIES  3
#define MAX_SEMNUM 255

#define CMD_OPEN     1
#define CMD_REMOVE   2
#define CMD_POST     3
#define CMD_POSTMULT 4
#define CMD_WAIT     5
#define CMD_TRYWAIT  6
#define CMD_SETVAL   7
#define CMD_GETVAL   8

#define ERR(e,s) fprintf(stderr, "%d: %s: %s\n", __LINE__, (s), strerror(e))

static const char *cmdnames[] = {
	NULL, "CMD_OPEN", "CMD_REMOVE", "CMD_POST", "CMD_POSTMULT",
	"CMD_WAIT", "CMD_TRYWAIT", "CMD_SETVAL", "CMD_GETVAL"
};

static const char *cmdflags = " orpmwtsg";

struct cmd {
	int cmd;
	int semid;
	int semnum;
	const char *path;
	int flags;
	int oflags;
	mode_t mode;
	int numsems;
	int value;
};

void
cmd_print(struct cmd *cmd)
{
	char oflags[255], *o = oflags;
	*o = '\0';
	if ((cmd->oflags & O_CREAT))
		o += sprintf(o, "O_CREAT");
	if ((cmd->oflags & O_EXCL))
		o += sprintf(o, "|O_EXCL");
	fprintf(stderr,
			"%s: semid: %d semnum: %d path: %s flags: %s oflags: %s mode: %04o numsems: %d value: %d\n",
			cmdnames[cmd->cmd], cmd->semid, cmd->semnum, cmd->path ? cmd->path : "",
			IS_SEMUNDO(cmd) ? "FLAGS_SEMUNDO" : "", oflags, cmd->mode, cmd->numsems, cmd->value);
}


#if defined(_SEM_SEMUN_UNDEFINED) || \
		(defined(__digital__) && defined(__unix__)) || \
		defined(_HPUX_SOURCE)
union semun {
	int val;                  /* value for SETVAL */
	struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
	unsigned short *array;    /* array for GETALL, SETALL */
	/* Linux specific part: */
	struct seminfo *__buf;    /* buffer for IPC_INFO */
};
#endif

/* Open or create a semaphore initializing it as necessary.
 */
static int
semid_get(const char *name, int nsems, int oflags, mode_t mode, int value)
{
	key_t key;
	int max;

	if (nsems > MAX_SEMNUM) {
		ERR(errno = ERANGE, "semid_get");
		return -1;
	}
	if ((key = ftok((char *)name, 1)) == (key_t)-1) {
		ERR(errno, "ftok");
		return -1;
	}

	/* This following loop ensures that we know if the semaphore was created
	 * as opposed to just opened so that it can be initialized properly. We
	 * do this by alternating between oflags 0 and IPC_CREATE|IPC_EXCL until
	 * one succeeds.
	 */ 
	for (max = MAX_TRIES; max; max--) {
		int semid;
		union semun arg;

		if ((oflags & O_EXCL) == 0) {
			if ((semid = semget(key, nsems, 0)) != -1) {
				struct semid_ds buf;

				/* This inner try-loop ensures that the semaphore is initialized before
				 * we return even if the semaphore has been created with semget but not
				 * yet initialized with semctl. See Stevens' UNPv2 p274.
				 */
				arg.buf = &buf;
				for (max = MAX_TRIES; max; max--) {
					if (semctl(semid, 0, IPC_STAT, arg) == -1) {
						ERR(errno, "semctl");
						return -1;
					}
					if (buf.sem_otime != 0) {
						return semid;
					}
					sleep(1);
				}

				ERR(errno = ETIMEDOUT, "semid_get");
				return -1;
			} else if (errno != ENOENT) {
				ERR(errno, "semget");
				return -1;
			}
		}
		if ((semid = semget(key, nsems, IPC_CREAT | IPC_EXCL | (mode & 0777))) != -1) {
			struct sembuf initop;

			if (nsems > 1) {
				unsigned short array[MAX_SEMNUM * sizeof(unsigned short)];
				int i;

				arg.array = array;
				arg.array[0] = 0; /* leave the first one 0 to be set with semop */
				for (i = 1; i < nsems; i++) {
					arg.array[i] = value;
				}
				if (semctl(semid, 0, SETALL, arg) == -1) {
					ERR(errno, "semctl");
					semctl(semid, 0, IPC_RMID);
					return -1;
				}
			} else {
				arg.val = 0;
				if (semctl(semid, 0, SETVAL, arg) == -1) {
					ERR(errno, "semctl");
					semctl(semid, 0, IPC_RMID);
					return -1;
				}
			}
                                 /* increment by value to set sem_otime nonzero */
			initop.sem_num = 0;
			initop.sem_op = value;
			initop.sem_flg = 0;
			if (semop(semid, &initop, 1) == -1) {
				ERR(errno, "semop");
				semctl(semid, 0, IPC_RMID);
				return -1;
			}

			return semid;
		} else if ((oflags & O_EXCL) || errno != EEXIST) {
			ERR(errno, "semget");
			return -1;
		}
	}

	ERR(errno = ETIMEDOUT, "semid_get");
	return -1;
}
static int
cmd_open(struct cmd *cmd)
{
	int semid, fd = 0;
	char oflags[255], *o = oflags;

	if ((cmd->oflags & O_CREAT) && (fd = open(cmd->path, O_CREAT, cmd->mode)) == -1) {
		ERR(errno, "open");
		return -1;
	}

	if ((semid = semid_get(cmd->path, cmd->numsems, cmd->oflags, cmd->mode, cmd->value)) == -1) {
		return -1;
	}
	*o = '\0';
	if ((cmd->oflags & O_CREAT))
		o += sprintf(o, "O_CREAT");
	if ((cmd->oflags & O_EXCL))
		o += sprintf(o, "|O_EXCL");
	fprintf(stderr, "%d = cmd_open(\"%s\", %d, %s, %04o, %d)\n",
			semid, cmd->path, cmd->numsems, oflags, cmd->mode, cmd->value);

	if (fd) close(fd);

	printf("%d", semid);

	return 0;
}
static int
cmd_remove(struct cmd *cmd)
{
	fprintf(stderr, "cmd_remove(%d)\n", cmd->semid);
	if (semctl(cmd->semid, 0, IPC_RMID) == -1) {
		ERR(errno, "semctl");
		return -1;
	}

	return 0;
}
static int
cmd_wait(struct cmd *cmd)
{
	struct sembuf wait;

	wait.sem_num = cmd->semnum;
	wait.sem_op = -1;
	wait.sem_flg = IS_SEMUNDO(cmd) ? SEM_UNDO : 0;

	fprintf(stderr, "cmd_wait(%d, %d, %s)\n",
			cmd->semid, cmd->semnum, IS_SEMUNDO(cmd) ? "SEM_UNDO" : "0");
	if (semop(cmd->semid, &wait, 1) == -1) {
		ERR(errno, "semop");
		return -1;
	}

	return 0;
}
static int
cmd_trywait(struct cmd *cmd)
{
	struct sembuf wait;

	wait.sem_num = cmd->semnum;
	wait.sem_op = -1;
	wait.sem_flg = IPC_NOWAIT | (IS_SEMUNDO(cmd) ? SEM_UNDO : 0);

	fprintf(stderr, "cmd_wait(%d, %d, %s)\n",
			cmd->semid, cmd->semnum, IS_SEMUNDO(cmd) ? "IPC_NOWAIT|SEM_UNDO" : "IPC_NOWAIT");
	if (semop(cmd->semid, &wait, 1) == -1) {
		ERR(errno, "semop");
		return errno == EAGAIN ? 0 : -1;
	}

	return 0;
}
static int
cmd_post(struct cmd *cmd)
{
	struct sembuf post;

	post.sem_num = cmd->semnum;
	post.sem_op = 1;
	post.sem_flg = IS_SEMUNDO(cmd) ? SEM_UNDO : 0;

	fprintf(stderr, "cmd_post(%d, %d, %s)\n",
			cmd->semid, cmd->semnum, IS_SEMUNDO(cmd) ? "SEM_UNDO" : "0");
	if (semop(cmd->semid, &post, 1) == -1) {
		ERR(errno, "semop");
		return -1;
	}

	return 0;
}
static int
cmd_post_multiple(struct cmd *cmd)
{
	struct sembuf post;
	int count = cmd->value;

	post.sem_num = cmd->semnum;
	post.sem_op = 1;
	post.sem_flg = IS_SEMUNDO(cmd) ? SEM_UNDO : 0;

	fprintf(stderr, "cmd_post_multiple(%d, %d, %s, %d)\n",
			cmd->semid, cmd->semnum, IS_SEMUNDO(cmd) ? "SEM_UNDO" : "0", count);
	while (count--) {
		if (semop(cmd->semid, &post, 1) == -1) {
			ERR(errno, "semop");
			return -1;
		}
	}

	return 0;
}
static int
cmd_getvalue(struct cmd *cmd)
{
	int v;

	if ((v = semctl(cmd->semid, cmd->semnum, GETVAL, 0)) == -1) {
		ERR(errno, "semctl");
		return -1;
	}
	fprintf(stderr, "%d = cmd_getvalue(%d, %d, GETVAL)\n", v, cmd->semid, cmd->semnum);

	return 0;
}
static int
cmd_setvalue(struct cmd *cmd)
{
	union semun arg;

	arg.val = cmd->value;
	if (semctl(cmd->semid, cmd->semnum, SETVAL, arg) == -1) {
		ERR(errno,"semctl");
		return -1;
	}
	fprintf(stderr, "cmd_setvalue(%d, %d, SETVAL)\n", cmd->semid, cmd->semnum);

	return 0;
}

typedef int (*cmd_fn)(struct cmd *cmd);
cmd_fn cmd_functions[] = {
	NULL,
	cmd_open,
	cmd_remove,
	cmd_post,
	cmd_post_multiple,
	cmd_wait,
	cmd_trywait,
	cmd_setvalue,
	cmd_getvalue
};

int
main(int argc, char *argv[])
{
	struct cmd cmd;

	if (argc < 4) {
usage:
		fprintf(stderr, "usage: semc <path|semid and semnum> -o|r|p|m|w|t|g|s <cmdargs>\n"
				"     EXAMPLES:\n"
				"         open: semc /tmp/foo -o 1 1 777 250 99\n"
                       /* argv: 0    1        2  3 4 5   6   7 */
				"       remove: semc 5441 I -r\n"
				"         post: semc 5441 3 -p\n"
				"post multiple: semc 5441 3 -m 10\n"
				"         wait: semc 5441 3 -w\n"
				"     try wait: semc 5441 3 -t\n"
				"          get: semc 5441 3 -g\n"
				"          set: semc 5441 3 -s 99\n");
		fputs("The -o command takes a path and command arguments O_CREAT and/or "
				"O_EXCL and/or 0, mode_t in octal, numsems, and the value with which "
				"the semaphore(s) should be initialize. All other commands take a semid "
				"and semnum but -r igores semnum.\n", stderr);
		return EXIT_FAILURE;
	}

	memset(&cmd, 0, sizeof cmd);
                                              /* Determine Command */
	if (strcmp(argv[2], "-o") == 0) {
		cmd.cmd = CMD_OPEN;
	} else if (*argv[3] == '-') {
		char *ch = strchr(cmdflags, argv[3][1]);
		if (!ch) {
			fprintf(stderr, "Invalid option: %s\n", argv[2]);
			goto usage;
		}
		cmd.cmd = ch - cmdflags;
	} else {
		fprintf(stderr, "Invalid options\n");
		goto usage;
	}
                             /* Populate struct cmd with Arguments */
	switch (cmd.cmd) {
		case CMD_OPEN:
			if (argc < 8) {
				fprintf(stderr, "Too few arguments for command\n");
				goto usage;
			}
			cmd.path = argv[1];
			if (*argv[3] == '1') cmd.oflags |= O_CREAT;
			if (*argv[4] == '1') cmd.oflags |= O_EXCL;
			{
				unsigned long mode;

				if ((mode = strtoul(argv[5], NULL, 8)) == ULONG_MAX) {
					ERR(errno, "strtoul");
					goto usage;
				}
				cmd.mode = mode;
			}
			if ((unsigned long)(cmd.numsems = strtoul(argv[6], NULL, 0)) == ULONG_MAX) {
				ERR(errno, "strtoul");
				goto usage;
			}
			if ((unsigned long)(cmd.value = strtoul(argv[7], NULL, 0)) == ULONG_MAX) {
				ERR(errno, "strtoul");
				goto usage;
			}
			break;
		case CMD_POSTMULT:
		case CMD_SETVAL:
			if ((unsigned long)(cmd.value = strtoul(argv[4], NULL, 0)) == ULONG_MAX) {
				ERR(errno, "strtoul");
				goto usage;
			}
		case CMD_REMOVE:
		case CMD_POST:
		case CMD_WAIT:
		case CMD_TRYWAIT:
		case CMD_GETVAL:
			if ((unsigned long)(cmd.semid = strtoul(argv[1], NULL, 0)) == ULONG_MAX ||
				(unsigned long)(cmd.semnum = strtoul(argv[2], NULL, 0)) == ULONG_MAX) {
				ERR(errno, "strtoul");
				goto usage;
			}
	}

	cmd_print(&cmd);
                                               /* Run the Command */
	if (cmd_functions[cmd.cmd](&cmd) == -1) {
		fprintf(stderr, "Command failed\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

