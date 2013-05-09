#!/bin/sh
if [ "$1" = "" ]; then
    echo wwwify.sh: must give project name as argument
    exit 1
fi
chmod -R go+r /usr/local/oracc/www/$1
