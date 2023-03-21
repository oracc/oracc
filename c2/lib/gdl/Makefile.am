include ../../../oracc-am-rules.txt

noinst_LTLIBRARIES=libgdl.la

libgdl_la_SOURCES = gdl.tab.c gdl-l.c gdlyacc.c gdl.c gdlxml.c sll.c gvl.c

YACC=bison

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../lib/std \
	-I../../lib/xml

gdl.tab.c gdl.tab.h: gdl.y gdl.l
	${YACC} -vd gdl.y

gdl-l.c: gdl.l gdl.tab.h
	flex -B -DYY_NO_INPUT gdl.l

gdl-l.o: gdl-l.c
