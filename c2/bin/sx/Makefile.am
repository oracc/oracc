include ../../../oracc-am-rules.txt

noinst_PROGRAMS=sx

xLDADD=\
	../../lib/cat/libcat.la \
	../../lib/std/libstd.la

AM_CPPFLAGS=\
	-I../../lib/std \
	-I../../lib/cat

sx_SOURCES=sx.c sx_check.c

sx_LDADD = $(xLDADD)

sx_DEPENDENCIES = $(sx_LDADD)
