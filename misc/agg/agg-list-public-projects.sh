#!/bin/sh
agg-test.sh || exit 1
xsltproc $ORACC/lib/scripts/agg-list-public-projects.xsl \
    $ORACC/agg/projects/public-projects.xml | sort -u
