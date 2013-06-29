#!/bin/sh
if [ -r oracc-resolver ]; then
    ./cgi-install.sh ${ORACC_VAR}/www/cgi-bin
fi
exit 0
