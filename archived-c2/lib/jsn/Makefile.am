include ../../../oracc-am-rules.txt

noinst_LTLIBRARIES=libjsn.la

libjsn_la_SOURCES = jsn.tab.c jsn-l.c jsn_writer.c jsonify.c

YACC=bison

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../lib/cat \
	-I../../lib/std \
	-I../../lib/sys \
	-I../../lib/xml

jsn.tab.c jsn.tab.h: jsn.y jsn.l
	${YACC} -vdt jsn.y

jsn-l.c: jsn.l jsn.tab.h
	flex -o jsn-l.c -d -B -DYY_NO_INPUT jsn.l

jsn-l.o: jsn-l.c
