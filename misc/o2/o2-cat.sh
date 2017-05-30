#!/bin/sh
mkdir -p 01bld/cat
rm -f 02www/index-cat.json
atflinks.plx >01bld/atflinks.tab
linknorm.plx 01bld/atflinks.tab >01bld/normlinks.tab
xmdmanager2.plx
xmd-check.plx
