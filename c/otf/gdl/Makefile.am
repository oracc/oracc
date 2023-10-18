include ../../../oracc-am-rules.txt

SUBDIRS=
noinst_LTLIBRARIES=libgdl.la

#
# gvl.c removed to use c2/gdl
#
libgdl_la_SOURCES = \
	graphemes.c inline.c c1c2gvl.c \
	gdlopts.c splitwords.c tokcheck.c tokenizer.c 

# cuneify.c gdl.c

AM_CPPFLAGS=\
	-I@top_srcdir@/c/otf/{atf,cdf,chr,lib,run,ox} \
	-I@top_srcdir@/c/include \
	-I@top_srcdir@/c2/lib/std \
	-I@top_srcdir@/c2/lib/sll \
	-I@top_srcdir@/c2/nss/gdl \
	-I@top_srcdir@/c/l2/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/lib/sas \
	-I@top_srcdir@/../oracc2/lib/{std,sys,lng,gvl,sll} \
	-I@top_srcdir@/../oracc2/include

EXTRA_DIST=*.h xvalue.c
