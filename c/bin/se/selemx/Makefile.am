## Process this file with automake to produce Makefile.in
##

.g.c:
	gperf -N `basename $*` -tT $< >$@

AM_CPPFLAGS=-I@top_srcdir@/c/bin/se \
	 -I@top_srcdir@/c/bin/se/selemx \
	-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/libstemmer_c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{chr,lng} \
	-I@top_srcdir@/c/l2/{form,link,ngram,psus,sigs,word,xcl2}

AM_CFLAGS=-pedantic -ansi -Wall -g

bin_PROGRAMS=selemx

selemx_SOURCES=selemx.c \
	indexed.c intercha.c option.c property.c branch.c \
	grapheme.c node.c parallel.c statnames.c signmap.c

selemx_LDADD = ../selib.o ../mm_any.o ../alias.o ../langmask.o \
	../propmask.o ../vid.o ../keymangler.o ../est.o \
	@top_srcdir@/c/otf/chr/libchr.la \
	@top_srcdir@/c/otf/lng/liblng.la \
	@top_srcdir@/c/l2/libl2.la \
	@top_srcdir@/c/lib/liboracc.la

EXTRA_DIST=fields.h index.h addgraph.c
