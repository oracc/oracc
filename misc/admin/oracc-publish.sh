#!/bin/sh
$project=`oraccopt`
if [ "$project" = "" ]; then
    echo oracc-publish.sh: unable to set project. Stop.
    exit 1
fi
touch ${ORACC}/Dropbox/messages/publish-$project
