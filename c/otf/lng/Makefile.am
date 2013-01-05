.g.c:
	gperf -N `basename $*` -tT $< >$@
AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=liblng.la

liblng_la_SOURCES=lang_atf.c lang_core.c langinst.c lang_mode.c \
	langstack.c langtag.c

INCLUDES=-I@top_srcdir@/c/include -I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{chr,lib,prj,run} \
	-I@top_srcdir@/c/lib/{oracc,sas} \
	-I@top_srcdir@/c/l2/{include,xli,form,xcl2,word,sigs,ngram,psus,link}

EXTRA_DIST=*.h
