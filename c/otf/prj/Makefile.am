SUFFIXES=.g .c .o

.g.c:
	gperf -N `basename $*` -tT $< >$@

AM_CFLAGS=-ansi -pedantic -Wall -g -std=c99
SUBDIRS=
noinst_LTLIBRARIES=libprj.la

libprj_la_SOURCES = \
	proj_load.c set_project.c

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/otf/{chr,gdl,lib,lng,run,ox} \
	-I@top_srcdir@/c/l2/include

EXTRA_DIST=proj_context.h
