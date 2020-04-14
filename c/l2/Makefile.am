.g.c:
	gperf -N `basename $*` -tT $< >$@
AM_CFLAGS=-pedantic -Wall -g
SUBDIRS=bigrams form ilem link ngram prop psus \
	sigs word xcl2 dsa nsa psa xli

noinst_LTLIBRARIES=libl2.la

libl2_la_DEPENDENCIES=sigs/libsigs.la word/libwords.la xcl2/libxcl2.la \
	form/libf2.la prop/libprops.la ilem/libilem.la \
	link/liblinks.la psus/libpsus.la bigrams/libbigrams.la \
	ngram/libngram.la xli/libxli.la \
	dsa/libdsa.la nsa/libnsa.la psa/libpsa.la psus/libpsus.la

libl2_la_LIBADD=sigs/libsigs.la word/libwords.la xcl2/libxcl2.la \
	form/libf2.la prop/libprops.la ilem/libilem.la \
	link/liblinks.la psus/libpsus.la bigrams/libbigrams.la \
	ngram/libngram.la xli/libxli.la \
	dsa/libdsa.la nsa/libnsa.la psa/libpsa.la psus/libpsus.la

EXTRA_DIST=l2.h
