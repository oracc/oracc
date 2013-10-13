#!/bin/sh
find * -maxdepth 0 -type d -name 'oracc-xmlrpc*' -exec rm -fr '{}' ';'
dir=oracc-xmlrpc-client-`cat VERSION`
mkdir $dir
cp  ../include/oraccnet.h \
    ../lib/callinfo.c ../lib/file.c ../lib/generic_request.c ../lib/util.c \
    ../lib/m_*_client.c \
    meths.c oracc-client.c result.c server-status.c *.sh \
    $dir
cp Makefile-client.* $dir
zip -r $dir.zip $dir
find * -maxdepth 0 -type d -name 'oracc-xmlrpc*' -exec rm -fr '{}' ';'
