#!/bin/sh
##
## This is a variant of oracc-user.sh for use with the new
## automated Oracc install
##
function fail {
    echo $*
    exit 1
}
uname=`uname`
if [ "$uname" = "Linux" ]; then
    sudo useradd -m -s /sbin/nologin oracc
    sudo passwd oracc
    sudo chmod -R g+rx /home/oracc
    sudo chmod -R go-w /home/oracc
    sudo chmod -R o-r /home/oracc
    printf '%s' oracc >.oraccuser
    printf '%s' oracc >.oraccgroup
else
    echo "$0: oracc multi-user mode only supported on Linux. Stop."
    exit 1
fi
