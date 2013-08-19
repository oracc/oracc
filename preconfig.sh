#!/bin/sh
if [ ! $(whoami) = "root" ]; then
    echo Oracc preconfiguration: must run as root. Stop
    exit 1
fi
httpduser=`./httpduser.sh`
if [ "$httpduser" = "" ]; then
    echo Oracc preconfiguration: unable to set httpduser. Stop.
    exit 1
fi
if [ ! -d $ORACC ]; then
    mkdir -p $ORACC
fi
if [ ! -d $ORACC/00/sys ]; then
    mkdir -p $ORACC/00/sys
fi
if [ ! -d /var/tmp/oracc/atfserver ]; then
    mkdir -p /var/tmp/oracc/{atfserver,pager}
fi
if [ ! -d /var/tmp/oracc/pager ]; then
    mkdir -p /var/tmp/oracc/pager
fi
chown -R $httpduser /var/tmp/oracc
chmod og-rwx /var/tmp/oracc/*
