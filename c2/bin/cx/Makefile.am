include ../../../oracc-am-rules.txt

noinst_PROGRAMS=

xLDADD=\
	../../lib/xml/libxml.la \
	../../lib/xml/rnv/librnv.la \
	../../lib/std/libstd.la \
	../../lib/sys/libsys.la \
	../../lem/frm/libfrm.la \
	../../lem/lng/liblng.la \
	../../nss/gdl/libgdl.la \
	../../lib/std/libstd.la \
	../../nss/gdl/libgdl.la \
	../../nss/cbd/libcbd.la \
	../../lib/std/libstd.la \
	../../lib/sys/libsys.la

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../lib/std \
	-I../../lib/sys \
	-I../../lib/xml \
	-I../../lem/lng \
	-I../../lem/frm \
	-I../../nss/gdl \
	-I../../nss/cbd

cx_SOURCES=

cx_LDADD = $(xLDADD)

cx_DEPENDENCIES = $(cx_LDADD)
