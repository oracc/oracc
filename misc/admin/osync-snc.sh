#!/bin/sh

project=$1

if [[ $project = "" ]]; then
    echo "osync-snc.sh: must give project. Stop."
    exit 1;
fi

if [[ $project == */* ]]; then
    echo "osync-snc.sh: project cannot be a subproject. Stop."
    exit 1;
fi

cd $ORACC_BUILDS

if [ ! -d $project ]; then 
    echo "osync-snc.sh: no such project $ORACC_BUILDS/$project. Stop."
    exit 1
fi

if [ ! -d snc ]; then
    echo "osync-snc.sh: $ORACC_BUILDS/snc doesn't exist. Stop."
    exit 1;
fi

tar zpcf snc/$project.tar.xz $project {bld,pub,tmp,www,xml}/$project
