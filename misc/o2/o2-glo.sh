#!/bin/sh
if [ -r 01bld/cancel ]; then
    echo Errors in glossary processing. Stop.
    exit 1
fi

l2p1.sh

if [ -r 01bld/cancel ]; then
    echo Errors in glossary processing. Stop.
    exit 1
fi

l2p2.sh

if [ -r 01bld/cancel ]; then
    echo Errors in glossary processing. Stop.
    exit 1
fi
