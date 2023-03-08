include ../../../oracc-am-rules.txt

noinst_LTLIBRARIES=libcat.la

libcat_la_SOURCES = cat.tab.c cat-l.c catyacc.c catstack.c cat.c

YACC=bison

AM_CPPFLAGS=\
	-I../../../c2/lib/std

cat.tab.c cat.tab.h: cat.y cat.l
	${YACC} -vd cat.y

cat-l.c: cat.l cat.tab.h
	flex -B -DYY_NO_INPUT cat.l

cat-l.o: cat-l.c
