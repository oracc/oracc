## Process this file with automake to produce Makefile.in
##

include ../../../../oracc-am-rules.txt

# build for secbdx is basically the same code base as setxtx

AM_CPPFLAGS=\
	-I@top_srcdir@/../oracc2/lib/{std,sys,lng} \
	-I@top_srcdir@/../oracc2/include \
	-I@top_srcdir@/c/bin/se \
	-I@top_srcdir@/c/bin/se/setxtx \
	-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/libstemmer_c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{chr,lng}

#VPATH=.:..:../setxtx


bin_PROGRAMS=secbdx

secbdx_SOURCES=secbdx.c statnames.c sn_alias.c

secbdx_LDADD=\
	@top_srcdir@/../oracc2/lib/liboracc2.la \
	../selib.o ../mm_any.o ../alias.o ../langmask.o \
	../setxtx/grapheme.o ../setxtx/node.o ../setxtx/parallel.o \
	../setxtx/indexed.o ../setxtx/intercha.o ../setxtx/gdlix.o \
	../setxtx/option.o ../setxtx/property.o ../est.o \
	../setxtx/branch.o \
	../vid.o ../propmask.o ../signmap.o \
	../rulestab.o ../keymangler.o ../v2.o \
	@top_srcdir@/c/otf/chr/libchr.la \
	@top_srcdir@/c/lib/liboracc.la

EXTRA_DIST=fields.h
