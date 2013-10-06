include ../../../../oracc-am-rules.txt

AM_CFLAGS=-ansi -pedantic -Wall -g
AM_CPPFLAGS=-I../include

bin_PROGRAMS=oracc-client oracc-build-client.sh

.g.c:
	gperf -N `basename $*` -tT $< >$@

meths.g: ../lib/m_*_client.c meths.sh
	./meths.sh

meths.c: meths.g meths.sh
	gperf -N `basename $*` -tT $< >$@

../lib/liboraccnet.la: ../lib/*.c ../include/*.h
	(cd ../lib ; $(MAKE) $(MAKE_FLAGS))

oracc-build-client.sh$(EXEEXT): helpers/oracc-build-client.sh.in
	$(ORACCIFY) helpers/oracc-build-client.sh.in

oracc_client_LDADD=@XMLRPC_CLIENT_LIBS@ -L../lib -loraccnet

oracc_client_SOURCES=oracc-client.c meths.c result.c server-status.c

EXTRA_oracc_client_DEPENDENCIES=../lib/liboraccnet.la
