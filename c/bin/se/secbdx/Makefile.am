## Process this file with automake to produce Makefile.in
##

# build for secbdx is basically the same code base as setxtx

.g.c:
	gperf -N$* -tT $< >$@

INCLUDES=-I@top_srcdir@/c/bin/se \
	 -I@top_srcdir@/c/bin/se/setxtx \
	-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/libstemmer_c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{chr,lng}

#VPATH=.:..:../setxtx

AM_CFLAGS=-pedantic -ansi -Wall -g

bin_PROGRAMS=secbdx

secbdx_SOURCES=secbdx.c statnames.c

secbdx_LDADD = ../selib.o ../mm_any.o ../alias.o ../langmask.o \
	../setxtx/grapheme.o ../setxtx/node.o ../setxtx/parallel.o \
	../setxtx/indexed.o ../setxtx/intercha.o \
	../setxtx/option.o ../setxtx/property.o ../est.o \
	../setxtx/branch.o ../rulestab.o ../keymangler.o \
	@top_srcdir@/c/otf/chr/libchr.la \
	@top_srcdir@/c/otf/lng/liblng.la \
	@top_srcdir@/c/lib/liboracc.la

EXTRA_DIST=fields.h
