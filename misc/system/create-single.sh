#!/bin/sh
shdir=${ORACC}/bin
project=$1
if [ "$project" = "" ];
then
    echo create-single.sh: must give project name as argument
    exit 1
fi

if [ "$ORACC_HOME" = "" ];
then
    echo create-single.sh: environment variable ORACC_HOME is not set
    exit 1
fi

if [ -d ${ORACC_HOME}/$project ];
then
    echo create-single.sh: user directory ${ORACC_HOME}/$project already exists
    exit 1
fi

if [ -d ${ORACC}/pub/$project ];
then
    echo create-single.sh: project directory ${ORACC}/pub/$project already exists
    exit 1
fi

mkdir -p ${ORACC_HOME}/$project

cd ${ORACC_HOME}/$project || (echo "create-single.sh: can't cd to ${ORACC_HOME}/$project" ; exit 1)

echo creating directories
for a in any atf bak bib bin cat lib map tei web xml ; do \
    mkdir -p 00$a ; \
done

mkdir -p 00lib/lists 00web/images
mkdir -p ${ORACC}/{bld,pub,tmp,www,xml}/$project
ln -sf ${ORACC_HOME}/$project ${ORACC}/
ln -sf ${ORACC}/bld/$project 01bld
ln -sf ${ORACC}/tmp/$project 01tmp
for a in pub www xml ; do \
  ln -sf ${ORACC}/$a/$project 02$a ; \
done
echo setting permissions
chmod u+rw ${ORACC}/{bld,pub,tmp,www,xml}/$project
chmod go+r ${ORACC}/{bld,pub,tmp,www,xml}/$project
chmod -R u+rw .
chmod -R g+r .
chmod -R o-rw *
echo done
exit 0
