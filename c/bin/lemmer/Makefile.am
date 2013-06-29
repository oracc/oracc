bin_PROGRAMS=bigrams lemmer ngrammer xis xis2

noinst_PROGRAMS=test

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/otf/{chr,lib,lng} \
	-I@top_srcdir@/c/l2/{form,ilem,link,ngram,prop,psus,sigs,word,xcl2,xli}

xLDADD= @top_srcdir@/c/l2/libl2.la \
	@top_srcdir@/c/otf/lng/liblng.la \
	@top_srcdir@/c/otf/chr/libchr.la \
	@top_srcdir@/c/lib/skl/libskl.la \
	@top_srcdir@/c/lib/liboracc.la

yLDADD= @top_srcdir@/c/lib/liboracc.la

test_SOURCES=test.c
test_LDADD=$(yLDADD)

bigrams_SOURCES=bigrams.c
bigrams_LDADD=$(yLDADD)

lemmer_SOURCES=lemmer.c
lemmer_LDADD=$(xLDADD)

ngrammer_SOURCES=ngrammer.c
ngrammer_LDADD=$(xLDADD)

xis_SOURCES=xis.c
xis_LDADD=$(xLDADD)

xis2_SOURCES=xis2.c
xis2_LDADD=$(xLDADD)

EXTRA_DIST=lemmer.h
