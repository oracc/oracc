#!/bin/sh
#
# First:
#  /usr/local/oracc/src/c/bin/se/se -s2 [SEARCH] >$listdir/results.lst \
#
# Then call this script with args:
#
#  s2-pipeline.sh LISTDIR PROJECT CETYPE
#
# e.g., s2-pipeline.sh /var/local/oracc/pager/s3DS4a7 etcsri -u
#
listdir=$1
project=$2
cetype=$3
wm -p$project $cetype <$listdir/results.lst | \
    s2-pg2wrapper.plx -p$project | \
    ce_xtf $cetype -p$project | \
    s2-ce_trim.plx | \
    xsltproc -stringparam project $project /usr/local/oracc/lib/scripts/s2-ce-xtf-HTML.xsl - \
    >$listdir/results.html
