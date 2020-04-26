# libmba example programs

prefix     = /usr/local
includedir = $(prefix)/include
libdir     = $(prefix)/lib

CFLAGS     = -c -g -W1 -I$(includedir) -L$(libdir) -lmba
CC         = ../mktool
PROGS      = toupper csvprint hexd semc calc

all: $(PROGS)

clean:
	rm -f $(PROGS)
