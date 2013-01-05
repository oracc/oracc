.g.c:
	gperf -N `basename $*` -tT $< >$@
AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libxli.la

libxli_la_SOURCES=xli.c

INCLUDES=-I@top_srcdir@/c/include -I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{lib,lng} \
	-I@top_srcdir@/c/l2/{form,ilem,link,ngram,prop,psus,sigs,word,xcl2,xli}


EXTRA_DIST = *.h xli_sux.c
