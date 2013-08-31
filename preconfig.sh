#!/bin/sh
if [ ! -r oraccenv.sh ]; then
    echo Oracc preconfiguration: please create oraccenv.sh before proceeding. Stop.
    exit 1
fi
. oraccenv.sh
if [ "$ORACC" = "" ]; then
    echo Oracc preconfiguration: please set ORACC variable in oraccenv.sh. Stop.
    exit 1
fi
if [ ! $(whoami) = "root" ]; then
    echo Oracc preconfiguration: must run as root. Stop.
    exit 1
fi
httpduser=`./httpduser.sh`
if [ "$httpduser" = "" ]; then
    echo 'Oracc preconfiguration: unable to set httpduser (do you need to install Apache?). Stop.'
    exit 1
fi
if [ ! -d $ORACC ]; then
    mkdir -p $ORACC
fi
if [ ! -d /var/tmp/oracc/atfserver ]; then
    mkdir -p /var/tmp/oracc/{atfserver,pager}
fi
if [ ! -d /var/tmp/oracc/pager ]; then
    mkdir -p /var/tmp/oracc/pager
fi
chown -R $httpduser:$httpduser /var/tmp/oracc
chmod og-rwx /var/tmp/oracc/*
