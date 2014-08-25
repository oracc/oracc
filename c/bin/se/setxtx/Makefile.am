## Process this file with automake to produce Makefile.in
##

include ../../../../oracc-am-rules.txt

.g.c:
	gperf -N `basename $*` -tT $< >$@

signmap.c: signmap.c.in
	$(ORACCIFY) signmap.c.in

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/bin/se \
	-I@top_srcdir@/c/bin/se/setxtx \
	-I@top_srcdir@/c/lib/libstemmer_c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{chr,lng}

AM_CFLAGS=-pedantic -ansi -Wall -g

bin_PROGRAMS=setxtx

setxtx_SOURCES=setxtx.c \
	indexed.c intercha.c option.c property.c branch.c \
	grapheme.c node.c parallel.c statnames.c signmap.c

setxtx_LDADD = ../selib.o ../mm_any.o ../alias.o ../langmask.o \
	../propmask.o ../vid.o ../est.o ../keymangler.o \
	@top_srcdir@/c/otf/chr/libchr.la \
	@top_srcdir@/c/otf/lng/liblng.la \
	@top_srcdir@/c/lib/liboracc.la

EXTRA_DIST=fields.h index.h addgraph.c
