#!/bin/sh
$project=`oraccopt`
if [ "$project" = "" ]; then
    echo oracc-preview.sh: unable to set project. Stop.
    exit 1
fi
touch ${ORACC}/Dropbox/messages/preview-$project
