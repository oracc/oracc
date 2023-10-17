include ../../../oracc-am-rules.txt

.g.c:
	gperf -N `basename $*` -tT $< >$@
SUBDIRS=
noinst_LTLIBRARIES=libpsus.la

psus.c: psus.c.in
	$(ORACCIFY) psus.c.in

libpsus_la_SOURCES=psu2.c links_psu.c

AM_CPPFLAGS=-I@top_srcdir@/c/include -I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{lib} \
	-I@top_srcdir@/c/l2/{form,ilem,link,ngram,prop,psus,sigs,word,xcl2,xli} \
	-I@top_srcdir@/../oracc2/lib/{std,lng}


EXTRA_DIST = *.h
