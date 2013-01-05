.g.c:
	gperf -N `basename $*` -tT $< >$@
AM_CFLAGS=-std=c99 -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libnsa.la
#noinst_PROGRAMS=nsatest

instdir=$(libdir)/data
inst_DATA=nm.xml

libnsa_la_SOURCES=nsa_init.c nsa_parse.c nsa_print.c nsa_mem.c sexnum.c \
	nsa_modern.c nsa_result.c nsa_xcl.c nsa_text.c nsa_sexnum.c \
	nsa_util.c nsa_ucount.c nsa_quantity.c nsa_measure.c \
	nsa_amount.c

#nsatest_SOURCES=nsatest.c
#nsatest_LDADD=libnsa.la ../xcl/libxcl.la \
#	../cdf/libcdf.la ../ngram/libngram.la ../links/liblinks.la ../lem/liblem.la ../xff/libxff.la \
#	../words/libwords.la \
#	../atf/libatf.la ../lib/libpsd.la ../rnv/librnv.la \
#	../skl/libskl.la ../sas/libsas.la ../expat/libexpat.la 

INCLUDES=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/l2/{form,ilem,link,ngram,prop,psus,sigs,word,xcl2}

EXTRA_DIST = *.h *.xml
