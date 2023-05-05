include ../../../../oracc-am-rules.txt

# noinst_LTLIBRARIES=libasl.la

# libasl_la_SOURCES = asl.tab.c asl-l.c aslyacc.c asl.c

bin_PROGRAMS=j2x

YACC=bison

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../lib/cat \
	-I../../lib/std \
	-I../../lib/sys \
	-I../../lib/xml

j2x.tab.c j2x.tab.h: j2x.y j2x.l
	${YACC} -vdt j2x.y

j2x-l.c: j2x.l j2x.tab.h
	flex -o j2x-l.c -d -B -DYY_NO_INPUT j2x.l

j2x-l.o: j2x-l.c

j2x_SOURCES=j2x.tab.c j2x-l.c j2x.c
