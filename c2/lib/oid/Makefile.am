include ../../../oracc-am-rules.txt

AM_CPPFLAGS=-I../include \
	-I../../lib/sys \
	-I../../lib/std

noinst_LTLIBRARIES=liboid.la

liboid_la_SOURCES=oid_load.c

noinst_PROGRAMS=oid

oid_SOURCES=oid.c
oid_LDADD=liboid.la ../std/libstd.la ../sys/libsys.la
