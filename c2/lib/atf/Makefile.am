include ../../../oracc-am-rules.txt

noinst_LTLIBRARIES=libatf.la

libatf_la_SOURCES = atf.tab.c atf-l.c atfyacc.c atftree.c atf.c

YACC=bison

AM_CPPFLAGS=\
	-I../../lib/cat \
	-I../../lib/std \
	-I../../lib/xml

atf.tab.c atf.tab.h: atf.y atf.l
	${YACC} -vd atf.y

atf-l.c: atf.l atf.tab.h
	flex -B -DYY_NO_INPUT atf.l

atf-l.o: atf-l.c
