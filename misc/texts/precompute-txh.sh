#!/bin/sh
if [ "$1" != "" ]; then
    cat $1 | xtf2txh -b
else
    xtf2txh -b
fi
