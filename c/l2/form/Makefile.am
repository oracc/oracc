.g.c:
	gperf -N `basename $*` -tT $< >$@
AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libf2.la

libf2_la_SOURCES=f2_parse.c f2_serialize.c f2_sig.c f2_alias.c f2_inherit.c

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/otf/{chr,gdl,lib,lng} \
	-I@top_srcdir@/c/lib/sas \
	-I@top_srcdir@/c/l2/{ilem,link,ngram,prop,psus,sigs,word,xcl2,xli}


EXTRA_DIST = *.h
