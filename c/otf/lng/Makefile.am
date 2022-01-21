include ../../../oracc-am-rules.txt

SUFFIXES=.g .c .o

AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=liblng.la

langinst.c: langinst.c.in
	$(ORACCIFY) langinst.c.in

liblng_la_SOURCES=lang_atf.c lang_core.c langinst.c lang_mode.c \
	langstack.c langtag.c langrtl.c

AM_CPPFLAGS=-I@top_srcdir@/c/include -I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/otf/{chr,lib,prj,run} \
	-I@top_srcdir@/c/lib/{oracc,sas} \
	-I@top_srcdir@/c/l2/{include,xli,form,xcl2,word,sigs,ngram,psus,link}

data_DATA=lang_core.g

EXTRA_DIST=*.h
