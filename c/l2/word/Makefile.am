.g.c:
	gperf -N `basename $*` -tT $< >$@

AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libwords.la

libwords_la_SOURCES=wordset.c

INCLUDES=-I@top_srcdir@/c/include

EXTRA_DIST=words.h
