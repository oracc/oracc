include ../../../oracc-am-rules.txt

bin_PROGRAMS=oracc-client

cgibin_PROGRAMS=oracc-xmlrpc-cgi

#oraccd-build 

oracc_client_LDADD=@XMLRPC_CLIENT_LIBS@

oracc_client_SOURCES=oracc-client.c

oracc_xmlrpc_cgi_LDADD=@XMLRPC_CGISERVER_LIBS@

oracc_xmlrpc_cgi_SOURCES=oracc-xmlrpc-cgi.c

#oraccd_build_SOURCES=daemonize.c error.c oraccd-build.c queue.c configure.c
#oraccd_build_LDADD=

EXTRA_DIST=apue.h
