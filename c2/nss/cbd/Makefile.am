include ../../../oracc-am-rules.txt

noinst_LTLIBRARIES=libcbd.la

libcbd_la_SOURCES = cbd.tab.c cbd-l.c cbdyacc.c cbd.c

YACC=bison

AM_CPPFLAGS=\
	-I../std \
	-I../include \
	-I../xml

cbd.tab.c cbd.tab.h: cbd.y cbd.l
	${YACC} -vd cbd.y

cbd-l.c: cbd.l cbd.tab.h
	flex -B -DYY_NO_INPUT cbd.l

cbd-l.o: cbd-l.c
