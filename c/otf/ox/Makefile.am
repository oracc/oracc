#
# This Makefile is for ox in an interim phase; bin/ox is no longer
# being built; instead we are building oxx which uses the c2
# gdl/gvl/lib
#

include ../../../oracc-am-rules.txt

SUBDIRS=
bin_PROGRAMS=oxx gdlme2x

bin_SCRIPTS=checksigs.sh

oxxLDADD = \
	../gdl/libgdl.la \
	../atf/libatf.la \
	../gdl/libgdl.la \
	../atf/libatf.la \
	../lib/libcdf.la \
	../chr/libchr.la \
	../otf/libotf.la \
	../prj/libprj.la \
	../run/librun.la \
	@top_srcdir@/c/l2/libl2.la \
	../chr/libchr.la \
	@top_srcdir@/c/lib/liboracc.la \
	@top_srcdir@/../oracc2/lib/liboracc2.la

oxx_SOURCES = \
	atffile.c help.c initterm.c odt_writer.c options.c \
	ox.c post_option_setup.c process_inputs.c globals.c

oxx_LDADD = $(oxxLDADD)
oxx_DEPENDENCIES = $(oxxLDADD)

gdlme2x_SOURCES = gdlme2x.c globals.c
gdlme2x_LDADD = \
	../gdl/libgdl.la \
	../atf/libatf.la \
	../gdl/libgdl.la \
	../atf/libatf.la \
	../lib/libcdf.la \
	../chr/libchr.la \
	../otf/libotf.la \
	../prj/libprj.la \
	../run/librun.la \
	@top_srcdir@/c/l2/libl2.la \
	../chr/libchr.la \
	@top_srcdir@/c/lib/liboracc.la \
	@top_srcdir@/../oracc2/lib/liboracc2.la

gdlme2x_DEPENDENCIES = $(gdlme2x_LDADD)

atffile.c: atffile.c.in
	$(ORACCIFY) atffile.c.in

initterm.c: initterm.c.in
	$(ORACCIFY) initterm.c.in

odt_writer.c: odt_writer.c.in
	$(ORACCIFY) odt_writer.c.in

post_option_setup.c: post_option_setup.c.in
	$(ORACCIFY) post_option_setup.c.in

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/otf/{atf,chr,gdl,lib,otf,prj,run} \
	-I@top_srcdir@/c/l2 \
	-I@top_srcdir@/c/l2/{bigrams,dsa,form,ilem,link,ngram,nsa,prop,psa,psus,sigs,word,xcl2,xli} \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/lib/rnv \
	-I@top_srcdir@/../oracc2/lib/{std,lng}
