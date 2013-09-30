include ../../../../oracc-am-rules.txt

.g.c:
	gperf -N `basename $*` -tT $< >$@

AM_CFLAGS=-D_BSD_SOURCE -ansi -pedantic -Wall -g
AM_CPPFLAGS=-I../include

noinst_LTLIBRARIES=liboraccnet.la

m_build_server.c: m_build_server.c.in
	$(ORACCIFY) m_build_server.c.in

m_ox_server.c: m_ox_server.c.in
	$(ORACCIFY) m_ox_server.c.in

util.c: util.c.in
	$(ORACCIFY) util.c.in

liboraccnet_la_SOURCES = \
	m_build_client.c m_build_server.c \
	m_debug_client.c m_debug_server.c \
	m_environment_client.c m_environment_server.c \
	m_ox_client.c m_ox_server.c \
	m_status_client.c m_status_server.c \
	callinfo.c \
	file.c \
	generic_request.c \
	request.c \
	sesh.c \
	util.c
