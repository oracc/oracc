#!/bin/sh
if [ "$1" == "" ]; then
    texmanager.plx
elif [ "$1" == "-d" ]; then
    texmanager.plx $*
else
    texmanager.plx -d $*
fi
