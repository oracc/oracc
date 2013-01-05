#!/bin/sh
if [ ! -e 00lib/config.xml ]; then
    echo freeze.sh: not in a project or subproject directory
    exit 1;
fi
if [ ! -r 00lib/config.xml ]; then
    echo freeze.sh: project is already frozen
    exit 1;
fi
chmod -x 00*
chmod +x 00lib
chmod -r 00lib/config.xml
