.g.c:
	gperf -N `basename $*` -tT $< >$@
AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libxcl2.la
noinst_PROGRAMS=

libxcl2_la_SOURCES = xcl_builder.c xcl_events.c xcltok.c xcl_map.c \
	xcl_chunkify.c xcl_head.c x2_serialize.c x2_debug.c \
	xcl_sparse.c

#xtfxcl.c

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/otf/{atf,chr,lib} \
	-I@top_srcdir@/c/l2/{form,ilem,link,ngram,prop,psus,sigs,word,xcl2,xli}

EXTRA_DIST=xcl.h
