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
# This is from the old days when a project was create with links
# into /usr/local/oracc vel sim: nowadays that is handled by the
# build process
#    cd ${ORACC}
#    chmod -fR o+r */$1
else
    echo "userperm.sh: no such project '$1'"
    exit 1
fi
