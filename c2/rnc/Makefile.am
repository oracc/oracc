include ../../oracc-am-rules.txt

noinst_LTLIBRARIES=librnc.la

rncdata_DATA=*.rnc

librnc_la_SOURCES=asl.c gdl.c xtf.c xpd.c

AM_CPPFLAGS=\
	-I../lib/xml

cleanch:
	rm -f *.c *.h
