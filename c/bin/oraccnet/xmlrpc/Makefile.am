include ../../../../oracc-am-rules.txt

AM_CFLAGS=-ansi -pedantic -Wall -g
AM_CPPFLAGS=-I../include

registry.c: ../lib/m_*_server.c
	./registry.sh

../lib/liboraccnet.la: ../lib/*.c ../include/*.h
	(cd ../lib ; $(MAKE) $(MAKE_FLAGS))

cgibin_PROGRAMS=oracc-xmlrpc

oracc_xmlrpc_LDADD=@XMLRPC_CGISERVER_LIBS@ -L../lib -loraccnet

oracc_xmlrpc_SOURCES=oracc-xmlrpc.c

EXTRA_oracc_xmlrpc_DEPENDENCIES=../lib/liboraccnet.la
