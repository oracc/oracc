## Process this file with automake to produce Makefile.in
##

include ../../../../oracc-am-rules.txt

AM_CPPFLAGS=\
	-I@top_srcdir@/../oracc2/lib/{include,std,sys,lng}
	-I@top_srcdir@/c/bin/se \
	-I@top_srcdir@/c/bin/se/selemx \
	-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/libstemmer_c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/l2/{form,link,ngram,psus,sigs,word,xcl2} \
	-I@top_srcdir@/c/otf/{chr,lng}


bin_PROGRAMS=selemx

selemx_SOURCES=selemx.c \
	indexed.c intercha.c option.c property.c branch.c \
	grapheme.c node.c parallel.c statnames.c

selemx_LDADD=\
	@top_srcdir@/../oracc2/lib/liboracc2.la \
	../selib.o ../mm_any.o ../alias.o ../langmask.o \
	../propmask.o ../vid.o ../keymangler.o ../est.o ../signmap.o ../v2.o \
	@top_srcdir@/c/otf/chr/libchr.la \
	@top_srcdir@/c/l2/libl2.la \
	@top_srcdir@/c/otf/chr/libchr.la \
	@top_srcdir@/c/lib/liboracc.la

EXTRA_DIST=fields.h index.h addgraph.c
