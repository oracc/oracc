## Process this file with automake to produce Makefile.in
##

# build for setrax is basically the same code base as setxtx

.g.c:
	gperf -L ANSI -N$* -tT $< >$@

AM_CPPFLAGS=\
	-I@top_srcdir@/../oracc2/lib/{include,std,sys,lng}
	-I@top_srcdir@/c/bin/se \
	-I@top_srcdir@/c/bin/se/setxtx \
	-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/libstemmer_c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{chr,lng}

#VPATH=.:..:../setxtx

AM_CFLAGS=-pedantic -Wall -g

bin_PROGRAMS=setrax

setrax_SOURCES=setrax.c

setrax_LDADD=\
	@top_srcdir@/../oracc2/lib/liboracc2.la \
	../selib.o ../mm_any.o ../alias.o ../langmask.o \
	../vid.o ../est.o ../v2.o \
	../setxtx/grapheme.o ../setxtx/node.o ../setxtx/parallel.o \
	../setxtx/indexed.o ../setxtx/intercha.o \
	../setxtx/option.o ../setxtx/property.o \
	../setxtx/branch.o ../rulestab.o ../keymangler.o \
	@top_srcdir@/c/otf/chr/libchr.la \
	@top_srcdir@/c/lib/liboracc.la

EXTRA_DIST=*.h
