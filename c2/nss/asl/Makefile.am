include ../../../oracc-am-rules.txt

noinst_LTLIBRARIES=libasl.la

libasl_la_SOURCES = asl.tab.c asl-l.c aslyacc.c asl.c \
		    asl_bld.c asl_listdef.c

# asltree.c

YACC=bison

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../lib/cat \
	-I../../lib/std \
	-I../../lib/sys \
	-I../../lib/sll \
	-I../../lib/xml \
	-I../../nss/gdl 

asl.tab.c asl.tab.h: asl.y asl.l
	${YACC} -vd asl.y

asl-l.c: asl.l asl.tab.h
	flex -d -B -DYY_NO_INPUT asl.l

asl-l.o: asl-l.c
