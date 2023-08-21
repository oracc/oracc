.g.c:
	gperf -N `basename $*` -tT $< >$@
AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libilem.la

bin_PROGRAMS=

## lemprop

## ilem_lnode.c

libilem_la_SOURCES=ilem_para.c ilem_parse.c \
	ilem_select.c ilem_unlemm.c ilem_wrapup.c \
	ilem_hnotes.c ilem_inherit.c ilem_props.c

AM_CPPFLAGS=-I@top_srcdir@/c/include -I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{atf,chr,gdl,lib,lng} \
	-I@top_srcdir@/c/l2/{form,link,ngram,prop,psus,sigs,word,xcl2,xli}

#lemprop_SOURCES = \
#	lemprop.c

#lemprop_LDADD = \
#	@top_srcdir@/c/lib/liboracc.la ilem_props.o

EXTRA_DIST = *.h
