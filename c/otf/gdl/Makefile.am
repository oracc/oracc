include ../../../oracc-am-rules.txt

SUBDIRS=
noinst_LTLIBRARIES=libgdl.la

libgdl_la_SOURCES = \
	cuneify.c gdl.c graphemes.c gvl.c inline.c \
	gdlopts.c splitwords.c tokcheck.c tokenizer.c

AM_CPPFLAGS=\
	-I@top_srcdir@/c/otf/{atf,cdf,chr,lib,lng,run,ox} \
	-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/l2/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/lib/sas

EXTRA_DIST=*.h xvalue.c
