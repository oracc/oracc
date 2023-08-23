include ../../../oracc-am-rules.txt

bin_PROGRAMS=sx slix slx
bin_SCRIPTS=ogsl-update.sh sxcmp.sh sxnorm.sh

xLDADD=\
	../../lem/lng/liblng.la \
	../../nss/asl/libasl.la \
	../../nss/gdl/libgdl.la \
	../../lib/collate/libcoll.la \
	../../lib/sll/libsll.la \
	../../lib/jsn/libjsn.la \
	../../lib/xml/libxml.la \
	../../lib/xml/rnv/librnv.la \
	../../lib/oid/liboid.la \
	../../lib/std/libstd.la \
	../../lib/collate/libcoll.la \
	../../rnc/librnc.la \
	../../lib/sys/libsys.la 

yLDADD=\
	../../lib/sys/libsys.la \
	../../lib/std/libstd.la 

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../lib/std \
	-I../../lib/collate \
	-I../../lib/jsn \
	-I../../lib/oid \
	-I../../lib/sll \
	-I../../lib/xml \
	-I../../lib/collate \
	-I../../lem/lng \
	-I../../rnc \
	-I../../nss/gdl \
	-I../../nss/asl \
	-I../../lib/sys

slx_SOURCES=slx.c

slx_LDADD = $(xLDADD)

slx_DEPENDENCIES = $(slx_LDADD)

sx_SOURCES=sx.c sx_marshall.c sx_homophones.c sx_compounds.c sx_digests.c \
	sx_inherited.c sx_listdefs.c sx_values.c sx_qualified.c sx_unicode.c \
	sx_walk.c sx_sll.c sx_w_asl.c sx_w_jsn.c sx_w_xml.c

sx_LDADD = $(xLDADD) -lpcre2-8

#sx_DEPENDENCIES = $(sx_LDADD)

slix_SOURCES=slix.c

slix_LDADD = $(yLDADD)

slix_DEPENDENCIES = $(s_index_LDADD)
