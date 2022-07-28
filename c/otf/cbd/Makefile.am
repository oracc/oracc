include ../../../oracc-am-rules.txt

#AM_CFLAGS=-ansi
AM_CFLAGS=-pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libcbd.la

bin_PROGRAMS=gx sx statis

libcbd_la_SOURCES = \
	common.c cbd.c header.c entry.c dcf.c form.c \
	sense.c inote.c isslp.c allow.c alias.c bff.c \
	parts.c bases.c	sig.c file.c note.c bib.c disc.c \
	cbdtags.c cbdpos.c sigops.c pleiades.c prop.c \
	stems.c root.c phon.c collo.c oid.c equiv.c \
	edit.c xgetline.c sigstats.c flex.c cbd_scanner.c

AM_CPPFLAGS=\
	-I@top_srcdir@/c/otf/{atf,cdf,chr,gdl,lib,lng,run,ox} \
	-I@top_srcdir@/c/include \
	-I@top_srcdir@/c/l2/form \
	-I@top_srcdir@/c/l2/ilem \
	-I@top_srcdir@/c/l2/link \
	-I@top_srcdir@/c/l2/ngram \
	-I@top_srcdir@/c/l2/psus \
	-I@top_srcdir@/c/l2/sigs \
	-I@top_srcdir@/c/l2/xcl2 \
	-I@top_srcdir@/c/l2/word \
	-I@top_srcdir@/c/lib/expat/xmlparse \
	-I@top_srcdir@/c/lib/sas

EXTRA_DIST=*.h xvalue.c

xLDADD = \
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
	./libcbd.la

cbd_scanner.c: cbd_scanner.l
	flex -DYY_NO_INPUT -o cbd_scanner.c cbd_scanner.l

gx_SOURCES = \
	gx.c globals.c 

gx_LDADD = $(xLDADD) -lfl

gx_DEPENDENCIES = $(xLDADD)

sx_SOURCES = \
	sx.c globals.c 

sx_LDADD = $(xLDADD)

sx_DEPENDENCIES = $(xLDADD)

statis_SOURCES = \
	statis.c

statis_LDADD = $(xLDADD)

statis_DEPENDENCIES = $(xLDADD)

