include ../../../oracc-am-rules.txt

bin_PROGRAMS=oid

xLDADD=\
	../../lib/oid/liboid.la \
	../../lib/sys/libsys.la \
	../../lib/std/libstd.la \
	../../lib/xml/libxml.la

AM_CPPFLAGS=\
	-I../../lib/include \
	-I../../lib/oid \
	-I../../lib/std \
	-I../../lib/sys 

oid_SOURCES=oid.c

oid_LDADD = $(xLDADD)

oid_DEPENDENCIES = $(oid_LDADD)
