include ../../../oracc-am-rules.txt

SUBDIRS=
noinst_LTLIBRARIES=libprj.la

set_project.c: set_project.c.in
	$(ORACCIFY) set_project.c.in

libprj_la_SOURCES = \
	default.c proj_load.c set_project.c

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/otf/{chr,gdl,lib,run,ox} \
	-I@top_srcdir@/c/l2/include \
	-I@top_srcdir@/../oracc2/lib/{std,lng}

EXTRA_DIST=proj_context.h
