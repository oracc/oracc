include ../../../oracc-am-rules.txt

SUBDIRS=expat rnv

AM_CPPFLAGS=\
	-I../include \
	-I../std \
	-I../sys

noinst_LTLIBRARIES=libxml.la

libxml_la_SOURCES = rnvif.c rnvval.c rnvxml.c runexpat.c xmlify.c xmlhelper.c tree_xml.c
