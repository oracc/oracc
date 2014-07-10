#!/bin/sh
project=$1
cd $ORACC/xml
if [ "$project" == "" ]; then
    for a in `find -name config.xml` ; do
	dirname $a | sed 's/^\.\///'
    done
else
    for a in `find $project -name config.xml` ; do
	dirname $a | sed 's/^\.\///' | grep ^$project
    done
fi
