#!/bin/sh
find * -maxdepth 0 -type d -name 'oracc-xmlrpc*' -exec rm -fr '{}' ';'
dir=oracc-xmlrpc-client-`cat VERSION`
mkdir $dir
chmod +x *-client.sh
cp  ../include/oraccnet.h \
    ../lib/callinfo.c ../lib/file.c ../lib/generic_request.c ../lib/util.c \
    ../lib/m_*_client.c \
    meths.c oracc-client.c result.c server-status.c \
    *-client.sh *-client-*.sh \
    $dir
cp Makefile-client.* $dir
chmod +x $dir/*.sh
rm -f $dir.zip
zip -r $dir.zip $dir
find * -maxdepth 0 -type d -name 'oracc-xmlrpc*' -exec rm -fr '{}' ';'
