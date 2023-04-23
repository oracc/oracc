include ../../../oracc-am-rules.txt

noinst_PROGRAMS=sx

xLDADD=\
	../../lib/gdl/libgdl.la \
	../../lib/asl/libasl.la \
	../../lib/xml/libxml.la \
	../../lib/xml/rnv/librnv.la \
	../../lib/std/libstd.la \
	../../lib/sys/libsys.la 

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../lib/asl \
	-I../../lib/gdl \
	-I../../lib/std \
	-I../../lib/sys \
	-I../../lib/xml 

sx_SOURCES=sx.c

sx_LDADD = $(xLDADD)

sx_DEPENDENCIES = $(sx_LDADD)
