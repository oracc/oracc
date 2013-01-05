#!/bin/sh
existdir=${ORACC}/lib/java/eXist
vartmp=/var/tmp/exist
(cd $vartmp ; rm -f nohup.out ; nohup $existdir/bin/startup.sh &)
