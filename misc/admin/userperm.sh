#!/bin/sh
if [ "$1" = "" ]; then
    echo userperm.sh: must give project name as argument
    exit 1
fi
if [ -d ${ORACC_HOME}/$1 ]; then
    cd ${ORACC_HOME}/$1
    chmod -fR u+rw .
    chmod -fR g+r .
    chmod -fR o-rw *
    cd ${ORACC}
    chmod -fR o+r */$1
else
    echo "userperm.sh: no such project '$1'"
    exit 1
fi
