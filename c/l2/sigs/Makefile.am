.g.c:
	gperf -N `basename $*` -tT $< >$@
AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libsigs.la

libsigs_la_SOURCES=sigs_context.c sigs_lib.c sigs_check.c \
	sigs_new.c sigs_lookup.c sigs_cache.c sigs_warn.c \
	sigs_cof.c sigs_dump_sigs.c sigs_show_insts.c

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/lib \
	-I@top_srcdir@/c/lib/sas \
	-I@top_srcdir@/c/otf/ox \
	-I@top_srcdir@/c/l2/{form,ilem,link,ngram,prop,psus,sigs,word,xcl2,xli} \
	-I@top_srcdir@/../oracc2/lib/{std,lng}


EXTRA_DIST = *.h
