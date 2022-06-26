include ../../../oracc-am-rules.txt

#AM_CFLAGS=-ansi
AM_CFLAGS=-pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libcbd.la

bin_PROGRAMS=gx

libcbd_la_SOURCES = \
	cbd_load.c

AM_CPPFLAGS=\
	-I@top_srcdir@/c/otf/{atf,cdf,chr,gdl,lib,lng,run,ox} \
	-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/l2/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/lib/sas

EXTRA_DIST=*.h xvalue.c

xLDADD = \
	../gdl/libgdl.la \
	../atf/libatf.la \
	../lib/libcdf.la \
	../chr/libchr.la \
	../lng/liblng.la \
	../otf/libotf.la \
	../prj/libprj.la \
	../run/librun.la \
	@top_srcdir@/c/l2/libl2.la \
	../chr/libchr.la \
	../lng/liblng.la \
	@top_srcdir@/c/lib/liboracc.la \
	./libcbd.la

gx_SOURCES = \
	gx.c globals.c common.c header.c entry.c

gx_LDADD = $(xLDADD)
