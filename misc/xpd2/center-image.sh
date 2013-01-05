#!/bin/sh
if [ "$1" == "" ]; then
    echo center-image.sh: need input filename and output filename
    exit 1
fi
if [ "$2" == "" ]; then
    echo center-image.sh: need output filename
    exit 1
fi
if [ ! -r $1 ]; then
    echo center-image.sh: no such input image $1
    exit 1
fi
convert $1 -gravity center -extent 600 -transparent white $2
exit 0
