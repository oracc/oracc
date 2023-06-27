include ../../../oracc-am-rules.txt

noinst_LTLIBRARIES=libcbd.la

libcbd_la_SOURCES = cbd.tab.c cbd-l.c cbdyacc.c \
	cbdpos.c cbdrws.c cbdtags.c

YACC=bison

AM_CPPFLAGS=\
	-I../../lib/std \
	-I../../lib/sys \
	-I../../lib/include \
	-I../../lem/frm \
	-I../../lem/lng \
	-I../../xml

cbd.tab.c cbd.tab.h: cbd.y cbd.l
	${YACC} -vdt cbd.y

cbd-l.c: cbd.l cbd.tab.h
	flex -d -B -DYY_NO_INPUT cbd.l
	grep '#define s_' cbd-l.c >startstates.h

cbd-l.o: cbd-l.c
