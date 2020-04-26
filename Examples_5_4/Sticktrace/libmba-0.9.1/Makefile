# Makefile for libmba
# Supported flags: -DVARRAY_INIT_SIZE=N -DUSE_WCHAR

MFLAGS     = -g -W1 -Isrc
MKTOOL     = ./mktool

prefix     = /usr/local
includedir = $(prefix)/include
libdir     = $(prefix)/lib
mandir     = $(prefix)/man
LIBNAME    = mba
LIBVERS    = 0.9.1
ARNAME     = lib$(LIBNAME).a
DISTRO     = lib$(LIBNAME)-$(LIBVERS)

OBJS       = src/dbug.o src/stack.o src/linkedlist.o src/hashmap.o src/hexdump.o src/msgno.o src/cfg.o src/pool.o src/varray.o src/shellout.o src/csv.o src/path.o src/misc.o src/text.o src/eval.o src/svsem.o src/allocator.o src/suba.o src/time.o src/bitset.o src/svcond.o src/daemon.o src/diff.o
PICOBJS    = $(OBJS:.o=.pic.o)
HDRS       = src/mba/dbug.h src/mba/msgno.h src/mba/stack.h src/mba/linkedlist.h src/mba/hashmap.h src/mba/hexdump.h src/mba/cfg.h src/mba/pool.h src/mba/varray.h src/mba/shellout.h src/mba/csv.h src/mba/iterator.h src/mba/text.h src/mba/path.h src/mba/eval.h src/mba/svsem.h src/mba/allocator.h src/mba/suba.h src/mba/time.h src/mba/bitset.h src/mba/svcond.h src/mba/daemon.h src/mba/diff.h src/mba/misc.h
MAN        = diff.3m bitset.3m allocator.3m cfg.3m pool.3m varray.3m csv.3m text.3m path.3m suba.3m msgno.3m stack.3m linkedlist.3m hashmap.3m hexdump.3m shellout.3m eval.3m svsem.3m svcond.3m time.3m
MANGZ      = $(MAN:.3m=.3m.gz)

.SUFFIXES: .pic.o .3m .3m.gz

all: mktool so ar

mktool:
	$(CC) -g -o mktool mktool.c

ar: mktool $(OBJS)
	ar $(ARFLAGS) $(ARNAME) $(OBJS)
	ranlib $(ARNAME)
so: mktool $(PICOBJS)
	@$(MKTOOL) -l -v -libname $(LIBNAME) -libvers $(LIBVERS) -shared -soname -lutil $(PICOBJS)

.c.pic.o:
	@$(MKTOOL) -c -v $(MFLAGS) -fpic -c -o $*.pic.o $<
.c.o:
	@$(MKTOOL) -c -v $(MFLAGS) -c -o $*.o $<

install: mktool
	-$(MKTOOL) -i $(ARNAME) $(libdir)
	-$(MKTOOL) -i -libname $(LIBNAME) -libvers $(LIBVERS) $(libdir)
	$(MKTOOL) -i $(HDRS) $(includedir)/mba
	$(MKTOOL) -i docs/man/*.3m.gz $(mandir)/man3
	@-libdir=$(libdir) mandir="$(mandir)" MAN="$(MAN)" MANGZ="$(MANGZ)" $(MKTOOL) -m -v platform_specific_install
	@echo
	@echo installation successful
uninstall: mktool
	$(MKTOOL) -u $(ARNAME) $(libdir)
	$(MKTOOL) -u -libname $(LIBNAME) -libvers $(LIBVERS) $(libdir)
	$(MKTOOL) -u $(HDRS) $(includedir)/mba
	rm -rf $(includedir)/mba
	$(MKTOOL) -u $(MANGZ) $(mandir)/man3
	-@libdir=$(libdir) mandir="$(mandir)" MAN="$(MAN)" MANGZ="$(MANGZ)" $(MKTOOL) -m -v platform_specific_uninstall
	@echo
	@echo de-installation successful

clean:
	$(MKTOOL) -C -libname $(LIBNAME) -libvers $(LIBVERS)
	rm -f $(ARNAME) src/*.o

zip:
	rm -rf /tmp/$(DISTRO) /tmp/$(DISTRO).zip
	cp -a . /tmp/$(DISTRO)
	cd /tmp/$(DISTRO) && crlf * docs/* docs/xml/* docs/www/* docs/ref/* examples/* examples/msgno/* examples/diff/* src/* src/mba/* tcase/* tcase/tests/* tcase/tests/data/*
	cd /tmp && zip -r $(DISTRO).zip $(DISTRO) -x "*/.*"

cal:
	$(MKTOOL) -c -g -I$(includedir) -L$(libdir) -lmba -rpath $(libdir) -o calc calc.c
