include ../../../oracc-am-rules.txt
SUBDIRS=
noinst_LTLIBRARIES=librun.la

librun_la_SOURCES = \
	run.c

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/otf/{chr,gdl,lib,prj} \
	-I@top_srcdir@/c/l2/include \
	-I@top_srcdir@/../oracc2/lib/{std,lng}

EXTRA_DIST=run.h
