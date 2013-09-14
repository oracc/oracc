include ../../../../oracc-am-rules.txt

.g.c:
	gperf -N `basename $*` -tT $< >$@

AM_CFLAGS=-ansi -pedantic -Wall -g
AM_CPPFLAGS=-I../include

noinst_LTLIBRARIES=liboraccnet.la

liboraccnet_la_SOURCES = \
	m_debug_client.c m_debug_server.c \
	m_environment_client.c m_environment_server.c \
	util.c
