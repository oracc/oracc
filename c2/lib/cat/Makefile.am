include ../../../oracc-am-rules.txt

noinst_LTLIBRARIES=libcat.la

libcat_la_SOURCES = cat.c cat-l.c cat.tab.c

YACC=bison

AM_CPPFLAGS=\
	-I../../../c/include

cat.tab.c cat.tab.h: cat.y cat.l
	${YACC} -vd cat.y

cat-l.c: cat.l cat.tab.h
	flex -B -DYY_NO_INPUT cat.l

cat-l.o: cat-l.c
