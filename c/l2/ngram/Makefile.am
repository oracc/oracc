.g.c:
	gperf -N `basename $*` -tT $< >$@
AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libngram.la

libngram_la_SOURCES=nl_load.c nl_dump.c ngramify.c nlcp_rewrite.c cts_tts.c

AM_CPPFLAGS=-I@top_srcdir@/c/include -I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{atf,chr,lib} \
	-I@top_srcdir@/c/l2/{xcl2,ngram,link,form,ilem,prop,psus,sigs,word} \
	-I@top_srcdir@/../oracc2/lib/{std,lng}

EXTRA_DIST=ngram.h
