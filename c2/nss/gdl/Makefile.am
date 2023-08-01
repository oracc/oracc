include ../../../oracc-am-rules.txt

noinst_LTLIBRARIES=libgdl.la

libgdl_la_SOURCES = gdl.tab.c gdl-l.c gdlyacc.c gdl.c gdlxml.c \
	gvl.c gvl_setup.c gvl_util.c gvl_s.c gvl_c.c gvl_q.c gvl_n.c \
	gdl_balance.c gdl_legacy.c gdl_lexfld.c gdl_mod.c gdlstate.c \
	gdl_word.c gdlsig.c gsort.c

YACC=bison

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../lib/collate \
	-I../../lib/sll \
	-I../../lib/std \
	-I../../lib/sys \
	-I../../lib/xml

gdl.tab.c gdl.tab.h: gdl.y gdl.l
	${YACC} -vdt gdl.y

gdl-l.c: gdl.l gdl.tab.h
	flex -d -B -DYY_NO_INPUT gdl.l

gdl-l.o: gdl-l.c
