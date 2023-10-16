include ../../../oracc-am-rules.txt

noinst_LTLIBRARIES=libxpd.la

libxpd_la_SOURCES = xpd.c

xpd.c: xpd.c.in
	$(ORACCIFY) xpd.c.in

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../lib/cat \
	-I../../lib/std \
	-I../../lib/sys \
	-I../../lib/xml
