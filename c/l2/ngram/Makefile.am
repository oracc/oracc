.g.c:
	gperf -N `basename $*` -tT $< >$@
AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libngram.la

libngram_la_SOURCES=nl_load.c nl_dump.c ngramify.c nlcp_rewrite.c cts_tts.c

INCLUDES=-I@top_srcdir@/c/include -I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{atf,chr,lib,lng} \
	-I@top_srcdir@/c/l2/{xcl2,ngram,link,form,ilem,psus,sigs,word}

EXTRA_DIST=ngram.h
