#!/bin/sh
project=`oraccopt`
if [ "$project" = "" ]; then
    echo oracc-publish.sh: unable to set project. Stop.
    exit 1
fi
version=`cat .version`
usproject=`/bin/echo -n $project | tr / _`
upload=${ORACC}/Dropbox/uploads/$usproject-$version
-r $upload || oracc-project-sources.sh $project $version
-r $upload && touch ${ORACC}/Dropbox/messages/publish-$usproject-$version
