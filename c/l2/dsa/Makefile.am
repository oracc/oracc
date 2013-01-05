.g.c:
	gperf -N `basename $*` -tT $< >$@
AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libdsa.la

libdsa_la_SOURCES=dsa.c cdlidate.c

INCLUDES=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/l2/{form,ilem,link,ngram,prop,psus,sigs,word,xcl2}

EXTRA_DIST=dsa.h cdlidate.h
