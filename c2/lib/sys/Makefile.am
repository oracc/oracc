include ../../../oracc-am-rules.txt

AM_CPPFLAGS=\
	-I../include \
	-I../sys

noinst_LTLIBRARIES=libsys.la

libsys_la_SOURCES = fname.c slurp.c utf.c sexify.c options.c xsystem.c loadfile.c

fname.c: fname.c.in
	$(ORACCIFY) $<
