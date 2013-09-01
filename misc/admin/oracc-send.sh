#!/bin/sh
project=`oraccopt`
if [ "$project" = "" ]; then
    oracc-send.sh: unable to set project.  Stop.
fi
usproject=`/bin/echo -n $project | tr / _`
oracc-project-sources.sh || exit 1
sent_sources=${ORACC}/Dropbox/sources/$usproject.tar.gz
this_sources=01tmp/$usproject.tar.gz
function copysent {
    cp -vf $this_sources $sent_sources
}
function filesize {
    `stat -s $1 | cut -d' ' -f8 | cut -d= -f2`
}
project=`oraccopt`
if [ "$project" = ""]; then
    echo oracc_send: unable to set project. Stop
fi
sent_size=`filesize $sent_sources`
this_size=`filesize $this_sources`
if [ "$sent_size" = "$this_size" ]; then
    cmp -s $sent_sources $this_sources || copysent
else
    copysent
fi
