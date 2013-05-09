#!/bin/sh
#
# First:
#  /usr/local/oracc/src/c/bin/se/se -s2 [SEARCH] >$listdir/results.lst \
#
# Then call this script with args:
#
#  s2-pipeline.sh LISTDIR PROJECT CETYPE
#
# e.g., s2-pipeline.sh /usr/local/oracc/pager/s3DS4a7 etcsri -u
#
listdir=$1
project=$2
cetype=$3
wm -p$project $cetype <$listdir/results.lst >$listdir/wm.out
s2-pg2wrapper.plx  -p$project <$listdir/wm.out >$listdir/pg2wrapper.out
ce_xtf $cetype -p$project <$listdir/pg2wrapper.out >$listdir/ce_xtf.out
s2-ce_trim.plx <$listdir/ce_xtf.out >$listdir/s2-ce_trim.xml
xsltproc -stringparam project $project /usr/local/oracc/lib/scripts/s2-ce-xtf-HTML.xsl \
    <$listdir/s2-ce_trim.xml >$listdir/results.html
