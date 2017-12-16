include ../../../oracc-am-rules.txt

.g.c:
	gperf -N `basename $*` -tT $< >$@
AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libbigrams.la

libbigrams_la_SOURCES=bigrams.c collos.c

bigrams.c: bigrams.c.in
	$(ORACCIFY) bigrams.c.in

collos.c: collos.c.in
	$(ORACCIFY) collos.c.in

AM_CPPFLAGS=-I@top_srcdir@/c/include -I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/{lib} \
	-I@top_srcdir@/c/l2/{form,ilem,ngram,link,psus,sigs,word,xcl2}


EXTRA_DIST = *.h
