#!/bin/sh
if [ ! -r oraccenv.sh ]; then
    echo Oracc preconfiguration: please create oraccenv.sh before proceeding. Stop.
    exit 1
fi
. ./oraccenv.sh
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
if [ ! -d $ORACC_BUILDS/tmp/sop ]; then
    mkdir -p $ORACC_BUILDS/tmp/sop
fi
mkdir -p $ORACC_BUILDS/htm
mkdir -p $ORACC_BUILDS/srv
mkdir -p $ORACC_BUILDS/snc
mkdir -p $ORACC_BUILDS/lib/data
if [ "$ORACC_MODE" = "single" ]; then
    cp usermode-single.xml $ORACC_BUILDS/lib/data/usermode.xml
    oraccuser=`cat .oraccuser`
else
    cp usermode-multi.xml $ORACC_BUILDS/lib/data/usermode.xml
    oraccuser=oracc
fi
. ./create_oracclocale_h.sh
mkdir -p $ORACC_BUILDS/{bld,xml,pub}
chown -R $httpduser:$oraccuser /var/tmp/oracc $ORACC_BUILDS/tmp/sop $ORACC_BUILDS/{bld,htm,pub,xml}
chmod +s $ORACC_BUILDS/{bld,htm,pub,xml}
chmod og-rwx /var/tmp/oracc/*
