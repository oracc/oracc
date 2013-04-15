#!/bin/sh
if [ "$1" = "" ]; then 
  echo delproj.sh: must give project name as first argument
  exit 1
fi

if [ "$ORACC_HOME" = "" ];
then
    echo create-single.sh: environment variable ORACC_HOME is not set
    exit 1
fi

if [ ! -d ${ORACC_HOME}/$1 ]; then
    echo delproj.sh: project $1 has no home, no tarball will be created
else
    cd ${ORACC_HOME} 
    mkdir -p oracc-deleted
    tar zcf oracc-deleted/$1.tar.gz $1
fi
rm -fr ${ORACC_HOME}/$1
rm -fr ${ORACC}/$1
rm -fr ${ORACC}/{bld,pub,tmp,www,xml}/$1
rm -f /var/spool/mail/$1
