#!/bin/sh
if [ "$1" == "" ]; then
    grep '^[-+>]\?\(@entry\|@sense\)'
else
    min=01tmp/`basename $1 .glo`.min
    grep '^[-+>]\?\(@entry\|@sense\)' $1 >$min
    echo $min
fi
