include ../../../oracc-am-rules.txt

#AM_CFLAGS=-ansi
AM_CFLAGS=-pedantic -Wall -g -O0
CFLAGS=
SUBDIRS=
noinst_LTLIBRARIES=libcbd.la

bin_PROGRAMS=gx sx statis

libcbd_la_SOURCES = \
	common.c sig.c cbdpos.c cbdrws.c sigops.c \
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

tg1.tab.c tg1.tab.h: tg1.y tg1.l
	${YACC} -vd tg1.y

tg1-l.c: tg1.l tg1.tab.h tg1.xnn
	flex -B -DYY_NO_INPUT tg1.l

tg1-l.o: tg1-l.c

tg1/tg1.rng: tg1.rnc
	mkdir -p tg1
	(cd tg1 ; java -jar ${javadir}/trang.jar ../tg1.rnc tg1.rng)

tg1.xnn: tg1/tg1.rng
	(cd tg1 ; ../rng2xnn.sh . >../tg1.xnn)

cbd_tg1_xnn.c: tg1.xnn rng2xnn.sh mk_xmlnames.plx
	./mk_xmlnames.plx -p tg1

tg2.tab.c tg2.tab.h: tg2.y tg2.l
	${YACC} -vd tg2.y

tg2-l.c: tg2.l tg2.tab.h tg2.xnn
	flex -B -DYY_NO_INPUT tg2.l

tg2-l.o: tg2-l.c

tg2/tg2.rng: tg2.rnc
	mkdir -p tg2
	(cd tg2 ; java -jar ${javadir}/trang.jar ../tg2.rnc tg2.rng)

tg2.xnn: tg2/tg2.rng
	(cd tg2 ; ../rng2xnn.sh . >../tg2.xnn)

cbd_tg2_xnn.c: tg2.xnn rng2xnn.sh mk_xmlnames.plx
	./mk_xmlnames.plx -p tg2

xc2/xc2.rng: xc2.rnc
	mkdir -p xc2
	(cd xc2 ; java -jar ${javadir}/trang.jar ../xc2.rnc xc2.rng)

xc2.xnn: xc2/xc2.rng rng2xnn.sh
	(cd xc2 ; ../rng2xnn.sh . >../xc2.xnn)

cbd_xc2_xnn.c: xc2.xnn mk_xmlnames.plx
	./mk_xmlnames.plx -p xc2

gx_SOURCES = \
	gx.c globals.c iomethod.c \
	tg1-l.c tg1.tab.c cbd_tg1_xnn.c \
	tg2-l.c tg2.tab.c cbd_tg2_xnn.c \
	cbd_xc2_xnn.c \
	rnvtgi.c rnvval.c rnvxml.c \
	builder.c iterator.c validate.c \
	o_tg2.c o_xc2.c 

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

