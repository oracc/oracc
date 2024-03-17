## Process this file with automake to produce Makefile.in
##

include ../../../../oracc-am-rules.txt

AM_CPPFLAGS=\
	-I@top_srcdir@/../oracc2/lib/{std,sys,lng} \
	-I@top_srcdir@/../oracc2/include \
	-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/bin/se \
	-I@top_srcdir@/c/bin/se/setxtx \
	-I@top_srcdir@/c/lib/libstemmer_c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{chr,lng}


bin_PROGRAMS=setxtx

setxtx_SOURCES=setxtx.c \
	indexed.c intercha.c option.c property.c branch.c \
	grapheme.c node.c parallel.c statnames.c gdlix.c

setxtx_LDADD = ../selib.o ../mm_any.o ../alias.o ../langmask.o \
	../propmask.o ../vid.o ../est.o ../keymangler.o ../signmap.o ../v2.o \
	@top_srcdir@/c/otf/chr/libchr.la \
	@top_srcdir@/c/lib/liboracc.la \
	@top_srcdir@/../oracc2/lib/liboracc2.la

EXTRA_DIST=fields.h index.h addgraph.c
