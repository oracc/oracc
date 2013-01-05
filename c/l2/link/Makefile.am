.g.c:
	gperf -N `basename $* $< >$@

AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=liblinks.la
#noinst_PROGRAMS=linkstest

liblinks_la_SOURCES = links_util.c links_dump.c links_chunks.c

#linkstest_SOURCES=linkstest.c
#linkstest_LDADD=liblinks.a ../lib/libpsd.a ../expat/libexpat.a

INCLUDES=-I@top_srcdir@/c/include -I@top_srcdir@/c/expat/xmlparse \
	-I@top_srcdir@/c/{cdf,n2} \
	-I@top_srcdir@/c/l2/{form,ilem,ngram,psus,sigs,word,xcl2}

EXTRA_DIST=links.h
