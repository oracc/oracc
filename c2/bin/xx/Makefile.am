SUBDIRS=j2x

include ../../../oracc-am-rules.txt

bin_PROGRAMS=concord tlitsig

xLDADD=\
	../../lem/lng/liblng.la \
	../../nss/gdl/libgdl.la \
	../../nss/asl/libasl.la \
	../../lib/xml/libxml.la \
	../../lib/collate/libcoll.la \
	../../lib/xml/rnv/librnv.la \
	../../lib/std/libstd.la \
	../../lib/sys/libsys.la 

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../lib/std \
	-I../../lib/xml \
	-I../../lem/lng \
	-I../../nss/gdl \
	-I../../rnc \
	-I../../lib/sys \
	-I../../../c/lib/collate

concord_SOURCES = concord.c

concord_LDADD = $(xLDADD)

concord_DEPENDENCIES = $(xLDADD)

tlitsig_SOURCES = tlitsig.c

tlitsig_LDADD = $(xLDADD)

tlitsig_DEPENDENCIES = $(xLDADD)
