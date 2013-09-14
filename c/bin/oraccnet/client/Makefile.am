include ../../../../oracc-am-rules.txt

AM_CPPFLAGS=-I../include

.g.c:
	gperf -N `basename $*` -tT $< >$@

meths.g: ../lib/m_*_client.c
	./meths.sh

meths.c: meths.g
	gperf -N `basename $*` -tT $< >$@

bin_PROGRAMS=oracc-client

oracc_client_LDADD=@XMLRPC_CLIENT_LIBS@ -L../lib -loraccnet

oracc_client_SOURCES=oracc-client.c meths.c
