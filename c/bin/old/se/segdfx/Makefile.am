## Process this file with automake to produce Makefile.in
##

.g.c:
	gperf -L ANSI -N `basename $*` -tT $< >$@

AM_CPPFLAGS=\
	-I@top_srcdir@/../oracc2/lib/{include,std,sys,lng} \
	-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/libstemmer_c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{chr,lng}

AM_CFLAGS=-pedantic -Wall -g

bin_PROGRAMS=segdfx

segdfx_SOURCES=segdfx.c
segdfx_LDADD=\
	@top_srcdir@/../oracc2/lib/liboracc2.la \
	../selib.o ../langmask.o \
	../rulestab.o ../keymangler.o ../est.o ../v2.o \
	@top_srcdir@/c/otf/chr/libchr.la \
	@top_srcdir@/c/lib/liboracc.la

EXTRA_DIST=fields.h
