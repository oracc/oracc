include ../../../oracc-am-rules.txt

#AM_CFLAGS=-ansi
AM_CFLAGS=-pedantic -Wall -g -O0
CFLAGS=
SUBDIRS=
noinst_LTLIBRARIES=libcbd.la

bin_PROGRAMS=gx sx statis

libcbd_la_SOURCES = \
	common.c sig.c cbdpos.c sigops.c \
	editscript.c xgetline.c sigstats.c msglist.c

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

grammar.tab.c grammar.tab.h: grammar.y scanner.l
	${YACC} -b grammar -vd grammar.y

scanner.c: scanner.l grammar.tab.h
	flex -B -DYY_NO_INPUT -o scanner.c scanner.l

scanner.o: scanner.c

cbd_rnc.c: cbd.rnc
	./mk_cbdrnc_c.plx >cbd_rnc.c

gx_SOURCES = \
	gx.c globals.c scanner.c grammar.tab.c \
	builder.c iterator.c identity.c validate.c \
	xmloutput.c cbd_rnc.c

gx_LDADD = -lfl $(xLDADD)

gx_DEPENDENCIES = $(xLDADD)

sx_SOURCES = \
	sx.c globals.c 

sx_LDADD = $(xLDADD)

sx_DEPENDENCIES = $(xLDADD)

statis_SOURCES = \
	statis.c

statis_LDADD = $(xLDADD)

statis_DEPENDENCIES = $(xLDADD)

