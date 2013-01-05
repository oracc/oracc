#!/bin/sh
project=`oraccopt`
if [ "$project" == "" ]; then
    echo urn-pqx.sh: must be used in a project directory
    exit 1
fi
rm -fr 02pub/urn ; mkdir -p 02pub/urn
urn-pqx.sh
#urn-sl.sh

