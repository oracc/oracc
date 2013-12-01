include ../../../../oracc-am-rules.txt

.g.c:
	gperf -N `basename $*` -tT $< >$@

AM_CFLAGS=-D_BSD_SOURCE -ansi -pedantic -Wall -g
AM_CPPFLAGS=-I../include

noinst_LTLIBRARIES=liboraccnet.la

m_build_server.c: m_build_server.c.in
	$(ORACCIFY) m_build_server.c.in

m_deploy_server.c: m_deploy_server.c.in
	$(ORACCIFY) m_deploy_server.c.in

m_ox_server.c: m_ox_server.c.in
	$(ORACCIFY) m_ox_server.c.in

varoracc.c: varoracc.c.in
	$(ORACCIFY) varoracc.c.in

liboraccnet_la_SOURCES = \
	m_build_client.c m_build_server.c \
	m_debug_client.c m_debug_server.c \
	m_deploy_client.c m_deploy_server.c \
	m_environment_client.c m_environment_server.c \
	m_ox_client.c m_ox_server.c \
	m_status_client.c m_status_server.c \
	callinfo.c \
	file.c file_b64.c \
	generic_request.c \
	request.c \
	sesh.c \
	util.c \
	varoracc.c
