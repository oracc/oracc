include ../../../oracc-am-rules.txt

noinst_PROGRAMS=sx

xLDADD=\
	../../lib/cat/libcat.la \
	../../lib/gdl/libgdl.la \
	../../lib/std/libstd.la \
	../../lib/sys/libsys.la \
	../../lib/xml/libxml.la \
	../../lib/xml/rnv/librnv.la

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../lib/gdl \
	-I../../lib/std \
	-I../../lib/sys \
	-I../../lib/xml \
	-I../../lib/cat

sx_SOURCES=sx.c sx_check.c sx_parse.c sl.c

sx_LDADD = $(xLDADD)

sx_DEPENDENCIES = $(sx_LDADD)
