#!/bin/sh
echo '### RUNNING ogsl-update.sh ###'
sx -c 00lib/ogsl.asl
if [ $? == 0 ]; then
    sx -b
    slix -b
    echo "ogsl-update.sh: ogsl data updated; web portal unchanged."
else
    echo "ogsl-update.sh: errors in ogsl processing. Stop."
    exit 1
fi
