include ../../../oracc-am-rules.txt

.g.c:
	gperf -N `basename $*` -tT $< >$@

SUBDIRS=
noinst_LTLIBRARIES=libxli.la

xli_sux.c: xli_sux.c.in
	$(ORACCIFY) xli_sux.c.in

xli.c: xli_sux.c

libxli_la_SOURCES=xli.c

AM_CPPFLAGS=-I@top_srcdir@/c/include -I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{lib,lng} \
	-I@top_srcdir@/c/l2/{form,ilem,link,ngram,prop,psus,sigs,word,xcl2,xli}


EXTRA_DIST = *.h xli_sux.c
