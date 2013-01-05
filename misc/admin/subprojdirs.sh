#!/bin/sh
if [ "$1" = "" ]; then
    echo subprojdirs.sh: must give project name as argument
    exit 1
fi
if [ ! -d 00lib ]; then
    echo subprojdirs.sh: this program must be run from an existing subproject
    exit 1
fi
if [ ! -e ../00lib/config.xml ]; then
    echo subprojdirs.sh: this program must be run from an existing project
    exit 1
fi
umask 0022
fullproj=$1
## we need to make the subdirs and link the proj's dirs back
## to them.
mkdir -p ${ORACC}/{bld,pub,tmp,www,xml}/$fullproj
[ -d 01bld ] || ln -sf ${ORACC}/bld/$fullproj 01bld
[ -d 01tmp ] || ln -sf ${ORACC}/tmp/$fullproj 01tmp
for a in pub www xml ; do \
    [ -d 02$a ] || ln -sf ${ORACC}/$a/$fullproj 02$a ; \
done
