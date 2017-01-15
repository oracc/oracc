#!/bin/sh
project=`oraccopt`
if [ "$project" == "" ]; then
    echo est-project.sh: must be called in a project directory
    exit 1
fi
rm -fr 01bld/est
mkdir -p 01bld/est
est-projuri.sh 2>01tmp/est-projuri.log
est-corpus.sh 2>01tmp/est-corpus.log
if [ -d 02www/cbd ]; then
    est-glo.sh 2>01tmp/est-glo.log
fi
est-project-web.sh
