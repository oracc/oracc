include ../../../oracc-am-rules.txt

SUBDIRS=

noinst_LTLIBRARIES=liblng.la

langinst.c: langinst.c.in
	$(ORACCIFY) langinst.c.in

liblng_la_SOURCES=lang_atf.c lang_core.c langinst.c lang_mode.c \
	langstack.c langtag.c langrtl.c

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../lib/std \
	-I../../lib/sys

data_DATA=lang_core.g

EXTRA_DIST=*.h
