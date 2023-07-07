include ../../../oracc-am-rules.txt

noinst_PROGRAMS=sx sl-index slx

xLDADD=\
	../../lem/lng/liblng.la \
	../../nss/gdl/libgdl.la \
	../../nss/asl/libasl.la \
	../../lib/sll/libsll.la \
	../../lib/xml/libxml.la \
	../../lib/xml/rnv/librnv.la \
	../../lib/std/libstd.la \
	../../lib/sys/libsys.la 

yLDADD=\
	../../lib/sys/libsys.la \
	../../lib/std/libstd.la 

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../lib/std \
	-I../../lib/sll \
	-I../../lib/xml \
	-I../../lem/lng \
	-I../../nss/gdl \
	-I../../lib/sys

slx_SOURCES=slx.c

slx_LDADD = $(xLDADD)

slx_DEPENDENCIES = $(slx_LDADD)

sx_SOURCES=sx.c

sx_LDADD = $(xLDADD)

sx_DEPENDENCIES = $(sx_LDADD)

sl_index_SOURCES=sl-index.c

sl_index_LDADD = $(yLDADD)

sl_index_DEPENDENCIES = $(s_index_LDADD)

