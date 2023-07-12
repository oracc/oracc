include ../../../oracc-am-rules.txt

bin_PROGRAMS=sx slix slx

xLDADD=\
	../../lem/lng/liblng.la \
	../../nss/asl/libasl.la \
	../../nss/gdl/libgdl.la \
	../../lib/sll/libsll.la \
	../../lib/xml/libxml.la \
	../../lib/xml/rnv/librnv.la \
	../../lib/std/libstd.la \
	../../rnc/librnc.la \
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
	-I../../rnc \
	-I../../nss/gdl \
	-I../../nss/asl \
	-I../../lib/sys

slx_SOURCES=slx.c

slx_LDADD = $(xLDADD)

slx_DEPENDENCIES = $(slx_LDADD)

sx_SOURCES=sx.c sx_asl.c sx_xml.c

sx_LDADD = $(xLDADD)

sx_DEPENDENCIES = $(sx_LDADD)

slix_SOURCES=slix.c

slix_LDADD = $(yLDADD)

slix_DEPENDENCIES = $(s_index_LDADD)

