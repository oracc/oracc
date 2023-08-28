include ../../../oracc-am-rules.txt

noinst_PROGRAMS=catx
bin_PROGRAMS=gdlx rocox

xLDADD=\
	../../lib/cat/libcat.la \
	../../nss/gdl/libgdl.la \
	../../lib/sll/libsll.la \
	../../lib/collate/libcoll.la \
	../../lib/std/libstd.la \
	../../lib/sys/libsys.la \
	../../lib/xml/libxml.la \
	../../lib/xml/rnv/librnv.la \
	../../rnc/librnc.la

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../nss/gdl \
	-I../../lib/collate \
	-I../../lib/sll \
	-I../../lib/std \
	-I../../lib/sys \
	-I../../lib/xml \
	-I../../lib/cat \
	-I../../rnc

catx_SOURCES=catx.c

catx_LDADD = $(xLDADD)

catx_DEPENDENCIES = $(xLDADD)

gdlx_SOURCES=gdlx.c

gdlx_LDADD = $(xLDADD)

gdlx_DEPENDENCIES = $(xLDADD)

rocox_SOURCES=rocox.c
rocox_LDADD = $(xLDADD)
rocox_DEPENDENCIES = $(xLDADD)
