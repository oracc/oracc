#!/bin/sh
what=$1
if [ "$what" == "" ]; then
    echo "$0: must say which oracc command, e.g., update. Stop"
    exit 1
fi
for a in `list-all-subs.sh` ; do
    (cd $a ; oracc $what)
done
oracc $what
