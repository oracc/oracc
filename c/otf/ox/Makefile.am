#
# This Makefile is for ox in an interim phase; bin/ox is no longer
# being build; instead we are building oxx which uses the c2
# gdl/gvl/lib
#

include ../../../oracc-am-rules.txt

#datadir=$(prefix)/lib/data

SUBDIRS=
bin_PROGRAMS=oxx

# gdlme2 sigx gdlsig gdlsname gv

# noinst_PROGRAMS=prj tok f2test
bin_SCRIPTS=checksigs.sh

xLDADD = \
	../gdl/libgdl.la \
	../atf/libatf.la \
	../gdl/libgdl.la \
	../atf/libatf.la \
	../lib/libcdf.la \
	../chr/libchr.la \
	../lng/liblng.la \
	../otf/libotf.la \
	../prj/libprj.la \
	../run/librun.la \
	@top_srcdir@/c/l2/libl2.la \
	../chr/libchr.la \
	../lng/liblng.la \
	@top_srcdir@/c/lib/liboracc.la \
	@top_srcdir@/c2/nss/gdl/libgdl.la \
	@top_srcdir@/c2/lib/std/libstd.la \
	@top_srcdir@/c2/lib/sys/libsys.la \
	@top_srcdir@/c/lib/liboracc.la

#ox_SOURCES = \
#	atffile.c help.c initterm.c odt_writer.c options.c \
#	ox.c post_option_setup.c process_inputs.c globals.c

#ox_LDADD = $(xLDADD)
#ox_DEPENDENCIES = $(ox_LDADD)

oxx_SOURCES = \
	atffile.c help.c initterm.c odt_writer.c options.c \
	ox.c post_option_setup.c process_inputs.c globals.c

oxx_LDADD = $(xLDADD)
oxx_DEPENDENCIES = $(ox_LDADD)

atffile.c: atffile.c.in
	$(ORACCIFY) atffile.c.in

initterm.c: initterm.c.in
	$(ORACCIFY) initterm.c.in

odt_writer.c: odt_writer.c.in
	$(ORACCIFY) odt_writer.c.in

post_option_setup.c: post_option_setup.c.in
	$(ORACCIFY) post_option_setup.c.in

#f2test_SOURCES = \
#	f2test.c globals.c
#
#f2test_LDADD = $(xLDADD)
#
#sigx_SOURCES = \
#	sigx.c globals.c
#
#sigx_LDADD = $(xLDADD)
#
#gdlme2_SOURCES = \
#	gdlme2.c globals.c
#
#gdlme2_LDADD = $(xLDADD)
#
#gdlsig_SOURCES = \
#	gdlsig.c globals.c
#
#gdlsig_LDADD = $(xLDADD)
#
#gdlsname_SOURCES = \
#	gdlsname.c globals.c
#
#gdlsname_LDADD = $(xLDADD)
#
#gv_SOURCES=gv.c \
#	@top_srcdir@/c/otf/atf/note.c \
#	@top_srcdir@/c/otf/ox/globals.c
#
#gv_LDADD= \
# 	@top_srcdir@/c/otf/gdl/libgdl.la \
# 	@top_srcdir@/c/otf/atf/libatf.la \
# 	@top_srcdir@/c/otf/gdl/libgdl.la \
# 	@top_srcdir@/c/otf/atf/libatf.la \
# 	@top_srcdir@/c/otf/chr/libchr.la \
# 	@top_srcdir@/c/otf/lib/libcdf.la \
# 	@top_srcdir@/c/otf/lng/liblng.la \
# 	@top_srcdir@/c/lib/liboracc.la

# prj_SOURCES = \
# 	prjtst.c globals.c

# prj_LDADD = \
# 	../lng/liblng.la \
# 	../prj/libprj.la \
# 	../run/librun.la \
# 	@top_srcdir@/c/lib/liboracc.la

# tok_SOURCES = \
# 	toktst.c globals.c

# tok_LDADD = $(xLDADD)

AM_CPPFLAGS=-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/otf/{atf,chr,gdl,lib,lng,otf,prj,run} \
	-I@top_srcdir@/c/l2 \
	-I@top_srcdir@/c/l2/{bigrams,dsa,form,ilem,link,ngram,nsa,prop,psa,psus,sigs,word,xcl2,xli} \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/lib/rnv \
	-I@top_srcdir@/c2/lib/std 

# data_DATA=ogsl.xml catpnums.vec

EXTRA_DIST=*.h $(inst_DATA)

