include ../../../oracc-am-rules.txt

SUFFIXES=.g .c .o

.g.c:
	gperf -N `basename $*` -tT $< >$@

#datadir=$(prefix)/lib/data

AM_CFLAGS=-pedantic -Wall -g -std=c99
SUBDIRS=
bin_PROGRAMS=ox gdlme2
noinst_PROGRAMS=prj tok

ox_SOURCES = \
	atffile.c help.c initterm.c odt_writer.c options.c \
	ox.c post_option_setup.c process_inputs.c globals.c

atffile.c: atffile.c.in
	$(ORACCIFY) atffile.c.in

initterm.c: initterm.c.in
	$(ORACCIFY) initterm.c.in

odt_writer.c: odt_writer.c.in
	$(ORACCIFY) odt_writer.c.in

post_option_setup.c: post_option_setup.c.in
	$(ORACCIFY) post_option_setup.c.in

xLDADD = \
	../atf/libatf.la \
	../gdl/libgdl.la \
	../chr/libchr.la \
	../lib/libcdf.la \
	../lng/liblng.la \
	../otf/libotf.la \
	../prj/libprj.la \
	../run/librun.la \
	@top_srcdir@/c/l2/libl2.la \
	@top_srcdir@/c/lib/liboracc.la

ox_LDADD = $(xLDADD)
ox_DEPENDENCIES = $(ox_LDADD)

gdlme2_SOURCES = \
	gdlme2.c globals.c

gdlme2_LDADD = $(xLDADD)

prj_SOURCES = \
	prjtst.c globals.c

prj_LDADD = \
	../lng/liblng.la \
	../prj/libprj.la \
	../run/librun.la \
	@top_srcdir@/c/lib/liboracc.la

tok_SOURCES = \
	toktst.c globals.c

tok_LDADD = $(xLDADD)

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/otf/{atf,chr,gdl,lib,lng,otf,prj,run} \
	-I@top_srcdir@/c/l2 \
	-I@top_srcdir@/c/l2/{bigrams,dsa,form,ilem,link,ngram,nsa,prop,psa,psus,sigs,word,xcl2,xli} \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/lib/rnv

# data_DATA=ogsl.xml catpnums.vec

EXTRA_DIST=*.h $(inst_DATA)

