include ../../../oracc-am-rules.txt

noinst_PROGRAMS=sx

xLDADD=\
	../../lib/cat/libcat.la \
	../../../c/lib/oracc/liboracc.la

AM_CPPFLAGS=\
	../../../c/include \
	-I../../lib/cat

sx_SOURCES=sx.c

sx_LDADD = $(xLDADD)

sx_DEPENDENCIES = $(sx_LDADD)
