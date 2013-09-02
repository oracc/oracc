#!/bin/sh
function copysent {
    cp -vf $*
    /bin/echo -n $newvers >.version
}
project=`oraccopt`
uploads=${ORACC}/Dropbox/uploads
if [ "$project" = "" ]; then
    echo oracc-upload.sh: unable to set project.  Stop.
fi
usproject=`/bin/echo -n $project | tr / _`
version=`cat .version`
if [ "$version" = "" ]; then
    version=1
    oracc-project-sources.sh $project $versions || exit 1
    curr_sources=01tmp/$usproject-$version.tar.gz
    copysent $curr_sources $uploads
else
    newvers=`expr $version + 1`
    oracc-project-sources.sh $project $newvers || exit 1
    sent_sources=$uploads/$usproject-$newvers.tar.gz
    curr_sources=01tmp/$usproject-$version.tar.gz
    sent_size=`filesize $sent_sources`
    curr_size=`filesize $curr_sources`
    if [ "$sent_size" = "$this_size" ]; then
	cmp -s $sent_sources $curr_sources || copysent $curr_sources $uploads
    else
	copysent $curr_sources $uploads
    fi
fi
