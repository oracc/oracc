include ../../../oracc-am-rules.txt

SUFFIXES=.g .c .o

.g.c:
	gperf -N `basename $*` -tT $< >$@

AM_CFLAGS=-ansi -pedantic -Wall -g -std=c99
SUBDIRS=
noinst_LTLIBRARIES=libatf.la

text.c: text.c.in
	$(ORACCIFY) text.c.in

libatf_la_SOURCES = \
	block.c blocktok.c label.c labtab.c \
	lemline.c nonx.c nonxtok.c \
	protocols.c text.c translate.c transtok.c \
	xtfxcl.c wordmatrix.c note.c

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/otf/{chr,gdl,lib,lng,prj,run,ox} \
	-I@top_srcdir@/c/l2 \
	-I@top_srcdir@/c/l2/{bigrams,dsa,form,ilem,link,ngram,nsa,prop,psa,psus,sigs,word,xcl2,xli}

EXTRA_DIST=*.h linenums.c
