include ../../../oracc-am-rules.txt

noinst_PROGRAMS=ax

xLDADD=\
	../../lib/cat/libcat.la \
	../../lib/gdl/libgdl.la \
	../../lib/std/libstd.la \
	../../lib/sys/libsys.la \
	../../lib/xml/libxml.la \
	../../lib/xml/expat/libexpat.la \
	../../lib/xml/rnv/librnv.la \
	../../lib/std/libstd.la \
	../../lib/atf/libatf.la

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../lib/atf \
	-I../../lib/cat \
	-I../../lib/gdl \
	-I../../lib/std \
	-I../../lib/sys \
	-I../../lib/xml

ax_SOURCES=ax.c ax_check.c

ax_LDADD = $(xLDADD)

ax_DEPENDENCIES = $(ax_LDADD)
