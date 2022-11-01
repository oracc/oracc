include ../../../oracc-am-rules.txt

AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libgsl.la

gsl.c: gsl.c.in
	$(ORACCIFY) gsl.c.in

libgsl_la_SOURCES = gsl.c

AM_CPPFLAGS=-I@top_srcdir@/c/include -I@top_srcdir@/c/lib/expat/xmlparse

# EXTRA_DIST=gsl.h
