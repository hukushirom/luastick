SECTION    = 3m
DATE       = "Apr 29, 2005"
LIB        = "libmba-0.9.1"
TITLE      = "MBA Library Functions"
COPYRIGHT  = "Michael B Allen <mba2000 ioplex.com>"
BASEREF    = "http://www.ioplex.com/~miallen/libmba/dl/docs/"
EDGE       = "\#808080"
MAINPANE   = "\#A2B5CD"
LEFTPANE   = "\#FFD700"
MIDDLEPANE = "\#f5f5f5"
FOOTER     = "\#808080"
TEXT1      = "\#000000"
TEXT2      = "\#000080"
XSLTCMD    = java -Dcom.jclark.xsl.sax.parser=com.jclark.xml.sax.Driver com.jclark.xsl.sax.Driver
XSLTARG    = section=$(SECTION) date=$(DATE) lib=$(LIB) title=$(TITLE) baseref=$(BASEREF)

XMLFILES   = xml/path.xml xml/diff.xml xml/cfg.xml xml/domnode.xml xml/hashmap.xml xml/hexdump.xml xml/linkedlist.xml xml/msgno.xml xml/stack.xml xml/pool.xml xml/shellout.xml xml/varray.xml xml/csv.xml xml/text.xml xml/text_details.xml xml/eval.xml xml/svsem.xml xml/allocator.xml xml/suba.xml xml/svcond.xml xml/bitset.xml xml/time.xml

MANFILES   = man/path.3m.gz man/diff.3m.gz man/hashmap.3m.gz man/linkedlist.3m.gz man/msgno.3m.gz man/cfg.3m.gz man/hexdump.3m.gz man/stack.3m.gz man/pool.3m.gz man/shellout.3m.gz man/varray.3m.gz man/csv.3m.gz man/text.3m.gz man/eval.3m.gz man/svsem.3m.gz man/suba.3m.gz man/allocator.3m.gz man/svcond.3m.gz man/bitset.3m.gz man/time.3m.gz

HTMLFILES  = ref/path.html ref/diff.html ref/domnode.html ref/hashmap.html ref/linkedlist.html ref/msgno.html ref/cfg.html ref/hexdump.html ref/stack.html ref/pool.html ref/shellout.html ref/varray.html ref/csv.html ref/text.html ref/text_details.html ref/sho.html ref/eval.html ref/svsem.html ref/suba.html ref/allocator.html ref/svcond.html ref/bitset.html ref/time.html


all: www/index.html man ref/libmba.html

man: $(MANFILES)

man/%.$(SECTION).gz : man/%.$(SECTION)
	gzip -f man/$*.$(SECTION)

man/%.$(SECTION) : xml/%.xml xml/man.xsl
	$(XSLTCMD) $< xml/man.xsl man/$*.$(SECTION) $(XSLTARG)

ref/libmba.html: $(XMLFILES) xml/libmba.xml xml/ref.xsl
	$(XSLTCMD) xml/libmba.xml xml/ref.xsl ref/libmba.html $(XSLTARG) color="#000080"

www/index.html: xml/index.xml xml/proj.xsl Makefile
	$(XSLTCMD) xml/index.xml xml/proj.xsl www/index.html $(XSLTARG) copyright=$(COPYRIGHT) edge=$(EDGE) mainpane=$(MAINPANE) leftpane=$(LEFTPANE) middlepane=$(MIDDLEPANE) footer=$(FOOTER) text1=$(TEXT1) text2=$(TEXT2)

genchar: genchar.c
	gcc -Wall -W -I../src ../src/csv.c -o genchar genchar.c

clean:
	$(RM) $(MANFILES) $(HTMLFILES) ref/libmba.html ref/toc.html ref/overview.html www/index.html

