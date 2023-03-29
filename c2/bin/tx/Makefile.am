include ../../../oracc-am-rules.txt

noinst_PROGRAMS=catx gdlx

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

catx_SOURCES=catx.c

catx_LDADD = $(xLDADD)

catx_DEPENDENCIES = $(xLDADD)

gdlx_SOURCES=gdlx.c

gdlx_LDADD = $(xLDADD)

gdlx_DEPENDENCIES = $(xLDADD)
