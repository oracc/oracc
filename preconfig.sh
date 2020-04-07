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

## KEEP ALL DIRECTORY CREATION HERE

./mkdirs.sh

if [[ $ORACC_MODE == "single" ]]; then
    ./prmdirs.sh
else
    chown -R $httpduser:$oraccgroup /var/tmp/oracc $ORACC_BUILDS/tmp/sop $ORACC_BUILDS/{bld,htm,pub,xml}
fi

chmod g+w /var/tmp/oracc $ORACC_BUILDS/tmp/sop $ORACC_BUILDS/{bld,htm,pub,xml}
chmod +s $ORACC_BUILDS/{bld,htm,pub,xml}
chmod og-rwx /var/tmp/oracc/*

if [ "$ORACC_MODE" = "single" ]; then
    cp usermode-single.xml $ORACC_BUILDS/lib/data/usermode.xml
    httpduser=`cat .oraccuser`
else
    cp usermode-multi.xml $ORACC_BUILDS/lib/data/usermode.xml
fi

oraccuser=`cat .oraccuser`
oraccgroup=`cat .oraccgroup`
. ./create_oracclocale_h.sh
