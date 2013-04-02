SUFFIXES=.g .c .o

.g.c:
	gperf -N `basename $*` -tT $< >$@

AM_CFLAGS=-pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libotf.la

libotf_la_SOURCES = \
	cdt_atf.c cdt_database.c cdt_data.c cdt_findfile.c \
	cdt_foreign.c cdt_inline.c cdt_reader.c cdt_style.c \
	cdttoks.c cdt_warning.c scan.c style_props.c

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/otf/{chr,gdl,lib,lng,prj,run,ox} \
	-I@top_srcdir@/c/l2/include

EXTRA_DIST=*.h
