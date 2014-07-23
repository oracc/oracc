#!/bin/sh
project=`oraccopt`
if [ "$project" == "" ]; then
    echo urn-pqx.sh: must be used in a project directory
    exit 1
fi
cut -d: -f2 02pub/cat/pqids.lst | sed 's/^/urn:oracc:/' >02pub/urn/pqx.urn
