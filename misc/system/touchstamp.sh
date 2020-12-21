#!/bin/sh
#replace a file with a zero-length file that has the same modification time as the original
f=$1
if [ -r $f ]; then
    if [ -w $f ]; then
	touch -r $f $f.T
	mv $f.T $f
    else
	echo "$0: file $f exists but is not writable"
	exit 1
    fi
else
    echo "$0: file $f not readable"
    exit 1
fi
