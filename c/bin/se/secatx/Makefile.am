## Process this file with automake to produce Makefile.in
##

include ../../../../oracc-am-rules.txt

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/libstemmer_c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{chr,lng}


bin_PROGRAMS=secatx

secatx_SOURCES=secatx.c statnames.c
secatx_LDADD = ../selib.o ../langmask.o \
	../rulestab.o ../keymangler.o ../vid.o ../est.o ../v2.o \
	@top_srcdir@/c/otf/chr/libchr.la \
	@top_srcdir@/c/otf/lng/liblng.la \
	@top_srcdir@/c/lib/liboracc.la

EXTRA_DIST=fields.h
