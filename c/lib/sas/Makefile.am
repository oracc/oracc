.g.c:
	gperf -N `basename $*` -tT $< >$@

AM_CFLAGS=-ansi -pedantic -Wall -g
SUBDIRS=
noinst_LTLIBRARIES=libsas.la

libsas_la_SOURCES = sas.c sas_asa.c

INCLUDES=-I@top_srcdir@/c/include -I@top_srcdir@/c/lib/expat/xmlparse

#EXTRA_DIST=sas.h
