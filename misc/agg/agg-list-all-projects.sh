#!/bin/sh
$ORACC/bin/agg-test.sh || exit 1
### YES: in fact agg-list-public-projects.xsl lists all the project names in the input
xsltproc --xinclude $ORACC/lib/scripts/agg-list-public-projects.xsl \
    $ORACC/agg/projects/all-projects.xml | sort -u
