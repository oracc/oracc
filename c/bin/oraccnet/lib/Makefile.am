include ../../../../oracc-am-rules.txt

.g.c:
	gperf -N `basename $*` -tT $< >$@

AM_CFLAGS=-ansi -pedantic -Wall -g
AM_CPPFLAGS=-I../include

noinst_LTLIBRARIES=liboraccnet.la

liboraccnet_la_SOURCES = \
	m_debug_client.c m_debug_server.c \
	m_environment_client.c m_environment_server.c \
	m_ox_client.c m_ox_server.c \
	callinfo.c \
	file.c \
	generic_request.c \
	request.c \
	sesh.c \
	util.c
