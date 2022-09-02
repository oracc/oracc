#!/bin/sh
while read line ; do
    rnv -r $1 $line
done
