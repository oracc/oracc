include ../../../oracc-am-rules.txt

.g.c:
	gperf -N `basename $*` -tT $< >$@
AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libcdf.la
noinst_SCRIPTS=mk_rnc.sh

libcdf_la_SOURCES = \
	catvec.c cdf_rnc.c linkcheck.c loadlabels.c \
	pnames.c rnvif.c tree.c xmlnames.c

catvec.c: catvec.c.in
	$(ORACCIFY) catvec.c.in

cdf_rnc.c: mk_rnc.sh
	/bin/sh	mk_rnc.sh

pnames.c: pnames.c.in
	$(ORACCIFY) pnames.c.in

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/l2/{bigrams,dsa,form,ilem,link,ngram,nsa,prop,psa,psus,sigs,word,xcl2,xli} \
	-I@top_srcdir@/c/l2 \
	-I@top_srcdir@/c/lib/sas \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/otf/lng

EXTRA_DIST=*.h

