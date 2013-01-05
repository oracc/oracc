#!/bin/sh
if [ "$1" = "" ]; then
    echo userperm.sh: must give project name as argument
    exit 1
fi
if [ -d ${ORACC_HOME}/$1 ]; then
    cd ${ORACC_HOME}/$1
    chmod -R u+rw .
    chmod -R g+r .
    chmod -R o-rw *
    cd ${ORACC}
    chmod -R o+r */$1
else
    echo "userperm.sh: no such project '$1'"
    exit 1
fi
