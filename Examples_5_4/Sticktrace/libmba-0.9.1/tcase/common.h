#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#define NDEBUG
#include <assert.h>
#if !defined(_WIN32)
#include <fcntl.h>
#include <sys/mman.h>
#endif
#include "mba/cfg.h"
#include "mba/csv.h"
#include "mba/hexdump.h"
#include "mba/linkedlist.h"
#include "mba/msgno.h"

#define BUFFER_SIZE_SM     256
#define BUFFER_MED_SM      8192
#define BUFFER_LRG_SM      0xFFFF

#define EXERCISE_SM_COUNT  1000
#define EXERCISE_MED_COUNT 10000
#define EXERCISE_LRG_COUNT 1000000
#define EXERCISE_SM_P1     200
#define EXERCISE_SM_P2     500
#define EXERCISE_SM_P3     750
#define EXERCISE_MED_P1    2000
#define EXERCISE_MED_P2    5000
#define EXERCISE_MED_P3    7500
#define EXERCISE_LRG_P1    200000
#define EXERCISE_LRG_P2    500000
#define EXERCISE_LRG_P3    750000
#define EXERCISE_R0        5
#define EXERCISE_R1        1
#define EXERCISE_R2        8
#define EXERCISE_R3        2

extern volatile sig_atomic_t sig;

extern int tcase_printf(int verbose, const char *fmt, ...);

int randint(int mn, int mx);
void *open_mmap(const char *name, int flags, int mode, size_t size);
int set_signals(void);

void rate_iterate(iter_t *iter);
int rate_next(iter_t *iter);

#endif /* COMMON_H */

