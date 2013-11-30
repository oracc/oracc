include ../../../../oracc-am-rules.txt

AM_CFLAGS=-ansi -pedantic -Wall -g
AM_CPPFLAGS=-I../include

cgibin_PROGRAMS=oracc-xmlrpc oracc-build-server.sh \
	oracc-server-environment.sh oracc-server-project-unpack.sh \
	oracc-server-web-pack.sh

oracc-xmlrpc.c: oracc-xmlrpc.c.in
	$(ORACCIFY) oracc-xmlrpc.c.in

registry.c: ../lib/m_*_server.c
	./registry.sh

oracc-build-server.sh$(EXEEXT): helpers/oracc-build-server.sh.in
	$(ORACCIFY) helpers/oracc-build-server.sh.in

oracc-server-environment.sh$(EXEEXT): helpers/oracc-server-environment.sh.in
	$(ORACCIFY) helpers/oracc-server-environment.sh.in

oracc-server-project-unpack.sh$(EXEEXT): helpers/oracc-server-project-unpack.sh.in
	$(ORACCIFY) helpers/oracc-server-project-unpack.sh.in

oracc-server-web-pack.sh$(EXEEXT): helpers/oracc-server-web-pack.sh.in
	$(ORACCIFY) helpers/oracc-server-web-pack.sh.in

../lib/liboraccnet.la: ../lib/*.c ../include/*.h
	(cd ../lib ; $(MAKE) $(MAKE_FLAGS))

oracc_xmlrpc_LDADD=@XMLRPC_CGISERVER_LIBS@ -L../lib -loraccnet

oracc_xmlrpc_SOURCES=oracc-xmlrpc.c status.c method_files.c

EXTRA_oracc_xmlrpc_DEPENDENCIES=../lib/liboraccnet.la
