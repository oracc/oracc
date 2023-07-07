include ../../../oracc-am-rules.txt

bin_PROGRAMS=ax

xLDADD=\
	../../nss/gdl/libgdl.la \
	../../nss/atf/libatf.la \
	../../lib/cat/libcat.la \
	../../lib/sys/libsys.la \
	../../lib/xml/libxml.la \
	../../lib/xml/expat/libexpat.la \
	../../lib/xml/rnv/librnv.la \
	../../lib/std/libstd.la \
	../../nss/gdl/libgdl.la \
	../../lib/std/libstd.la \
	../../lib/sll/libsll.la \
	../../lib/sys/libsys.la

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../lib/cat \
	-I../../nss/atf \
	-I../../nss/gdl \
	-I../../lib/sll \
	-I../../lib/std \
	-I../../lib/sys \
	-I../../lib/xml

ax_SOURCES=ax.c ax_check.c ax_parse.c

ax_LDADD = $(xLDADD)

ax_DEPENDENCIES = $(ax_LDADD)
