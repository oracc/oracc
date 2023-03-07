include ../../../oracc-am-rules.txt

noinst_PROGRAMS=ax

xLDADD=\
	../../lib/cat/libcat.la \
	../../../c/lib/oracc/liboracc.la

AM_CPPFLAGS=\
	../../../c/include \
	-I../../lib/cat

ax_SOURCES=ax.c

ax_LDADD = $(xLDADD)

ax_DEPENDENCIES = $(ax_LDADD)
