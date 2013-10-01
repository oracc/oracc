include ../../../../oracc-am-rules.txt

AM_CFLAGS=-ansi -pedantic -Wall -g
AM_CPPFLAGS=-I../include

cgibin_PROGRAMS=oracc-xmlrpc oracc-build-server.sh

oracc-xmlrpc.c: oracc-xmlrpc.c.in
	$(ORACCIFY) oracc-xmlrpc.c.in

registry.c: ../lib/m_*_server.c
	./registry.sh

oracc-build-server.sh$(EXEEXT): helpers/oracc-build-server.sh.in
	$(ORACCIFY) helpers/oracc-build-server.sh.in

../lib/liboraccnet.la: ../lib/*.c ../include/*.h
	(cd ../lib ; $(MAKE) $(MAKE_FLAGS))

oracc_xmlrpc_LDADD=@XMLRPC_CGISERVER_LIBS@ -L../lib -loraccnet

oracc_xmlrpc_SOURCES=oracc-xmlrpc.c status.c method_files.c

EXTRA_oracc_xmlrpc_DEPENDENCIES=../lib/liboraccnet.la
