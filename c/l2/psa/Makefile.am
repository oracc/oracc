.g.c:
	gperf -N `basename $*` -tT $< >$@
AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=

instdir=$(libdir)/data
inst_DATA=psa.xml

noinst_LTLIBRARIES=libpsa.la
libpsa_la_SOURCES=psa.c

INCLUDES=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/l2/{form,ilem,link,ngram,prop,psus,sigs,word,xcl2}

EXTRA_DIST=psa.h *.xml
