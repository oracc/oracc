include ../../../../oracc-am-rules.txt

AM_CFLAGS=-ansi -pedantic -Wall -g
AM_CPPFLAGS=-I../include

cgibin_PROGRAMS=oracc-xmlrpc xmlrpc-build.sh

oracc-xmlrpc.c: oracc-xmlrpc.c.in
	$(ORACCIFY) oracc-xmlrpc.c.in

registry.c: ../lib/m_*_server.c
	./registry.sh

xmlrpc-build.sh: xmlrpc-build.sh.in
	$(ORACCIFY) xmlrpc-build.sh.in

../lib/liboraccnet.la: ../lib/*.c ../include/*.h
	(cd ../lib ; $(MAKE) $(MAKE_FLAGS))

oracc_xmlrpc_LDADD=@XMLRPC_CGISERVER_LIBS@ -L../lib -loraccnet

oracc_xmlrpc_SOURCES=oracc-xmlrpc.c status.c

EXTRA_oracc_xmlrpc_DEPENDENCIES=../lib/liboraccnet.la
