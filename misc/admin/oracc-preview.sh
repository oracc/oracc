#!/bin/sh
project=`oraccopt`
if [ "$project" = "" ]; then
    echo oracc-preview.sh: unable to set project. Stop.
    exit 1
fi
version=`cat .version`
usproject=`/bin/echo -n $project | tr / _`
-r ${ORACC}/Dropbox/uploads/$usproject-$version \
    || oracc-project-sources $project $version
touch ${ORACC}/Dropbox/messages/preview-$usproject-$version
