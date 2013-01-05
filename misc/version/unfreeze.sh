#!/bin/sh
if [ ! -e 00lib/config.xml ]; then
    echo unfreeze.sh: not in a project or subproject directory
    exit 1;
fi
if [ -r 00lib/config.xml ]; then
    echo unfreeze.sh: project is not frozen
    exit 1;
fi
chmod +x 00*
chmod +r 00lib/config.xml
