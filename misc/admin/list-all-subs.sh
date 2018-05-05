#!/bin/sh
function fail {
    echo "$*. Stop."
    exit 1
}

if [ ! -r 00lib/config.xml ]; then
    fail "no 00lib/config.xml"
fi

find . -type d -name 00lib -exec ls '{}/config.xml' ';' \
    | sed 's#/00lib/config.xml##g' \
    | grep -v 00any | sed 's#^./##g' | grep -v '^.$'
