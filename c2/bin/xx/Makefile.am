SUBDIRS=j2x

include ../../../oracc-am-rules.txt

bin_PROGRAMS=concord

xLDADD=\
	../../../c/lib/liboracc.la \
	../../lib/cat/libcat.la \
	../../nss/gdl/libgdl.la \
	../../nss/atf/libatf.la \
	../../lib/sys/libsys.la \
	../../lib/xml/libxml.la \
	../../lib/xml/expat/libexpat.la \
	../../lib/xml/rnv/librnv.la \
	../../lib/std/libstd.la

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../lib/atf \
	-I../../lib/cat \
	-I../../lib/gdl \
	-I../../lib/std \
	-I../../lib/sys \
	-I../../lib/xml \
	-I../../../c/lib/collate

concord_SOURCES = concord.c

concord_LDADD = $(xLDADD)

