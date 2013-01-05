#!/bin/sh
xsltproc $ORACC/lib/scripts/agg-list-public-projects.xsl \
    $ORACC/agg/projects/public-projects.xml | sort -u
