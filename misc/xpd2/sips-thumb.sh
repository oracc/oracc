#!/bin/sh
if [ ! -d 00lib ]; then
    echo sips-thumb.sh: no 00lib directory. Stop.
    exit 1
fi
if [ "$1" == "" ]; then
    echo sips-thumb.sh: must give image file on command line. Stop.
    exit 1
fi
f=$1
b="${f##*.}"
if [ "$b" != "png" ]; then
    echo sips-thumb.sh: image must be in png format. Stop.
    exit 1
fi
if [ ! -r $f ]; then
    echo sips-thumb.sh: image file $f is not readable. Stop.
    exit 1
fi
cp -fp $f 00lib/thumb.png
if [ -x `which sips` ]; then
    sips -Z 66 00lib/thumb.png >/dev/null 2>&1
else
    echo sips-thumb.sh: sorry, no sips on this machine \(not a Mac?\). Stop.
fi
