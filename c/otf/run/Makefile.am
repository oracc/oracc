SUFFIXES=.g .c .o

.g.c:
	gperf -N `basename $*` -tT $< >$@

AM_CFLAGS=-ansi -pedantic -Wall -g -std=c99
SUBDIRS=
noinst_LTLIBRARIES=librun.la

librun_la_SOURCES = \
	run.c

INCLUDES=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/otf/{chr,gdl,lib,lng,prj} \
	-I@top_srcdir@/c/l2/include

EXTRA_DIST=run.h
