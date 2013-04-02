.g.c:
	gperf -N `basename $*` -tT $< >$@
AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libilem.la

libilem_la_SOURCES=ilem_para.c ilem_parse.c ilem_lnode.c \
	ilem_select.c ilem_unlemm.c ilem_wrapup.c \
	ilem_hnotes.c ilem_inherit.c

AM_CPPFLAGS=-I@top_srcdir@/c/include -I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{atf,chr,gdl,lib,lng} \
	-I@top_srcdir@/c/l2/{form,link,ngram,prop,psus,sigs,word,xcl2,xli}


EXTRA_DIST = *.h
