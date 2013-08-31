#!/bin/sh
function fail {
    echo $*
    exit 1
}
uname=`uname`
if [ "$uname" = "Linux" ]; then
    sudo useradd oracc
    passwd oracc
fi
su oracc oracc-source.sh
