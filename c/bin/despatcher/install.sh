#!/bin/sh
if [ "$1" = "" ]; then
    www=${ORACC}/www/cgi-bin
else
    www=$1
fi
if [ -d $www ]; then
    cp -f oracc-resolver $www/oracc-despatcher
    cd $www
    chmod uga-w oracc-despatcher
    chmod o+rx oracc-despatcher
fi
