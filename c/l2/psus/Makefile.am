include ../../../oracc-am-rules.txt

.g.c:
	gperf -N `basename $*` -tT $< >$@
AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libpsus.la

psus.c: psus.c.in
	$(ORACCIFY) psus.c.in

libpsus_la_SOURCES=psu2.c links_psu.c

AM_CPPFLAGS=-I@top_srcdir@/c/include -I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{lib,lng} \
	-I@top_srcdir@/c/l2/{form,ilem,link,ngram,prop,psus,sigs,word,xcl2,xli}


EXTRA_DIST = *.h
