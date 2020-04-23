#!/bin/sh
project=$1
if [[ $project = "epsd2" ]]; then
    echo l2p2-new 00bin/awp-sense-sort.plx epsd2
    00bin/awp-sense-sort.plx epsd2
    echo l2p2-new 00bin/awp-base-sort.plx epsd2
    00bin/awp-base-sort.plx epsd2
    echo l2p2-new redoing summaries
    xsltproc -stringparam projectDir $ORACC_BUILDS/$project \
	     -o 01bld/sux/summaries.xml \
	     $ORACC_BUILDS/lib/scripts/g2-summaries.xsl \
	     $ORACC_BUILDS/$project/01bld/sux/articles-with-periods.xml
else
    echo l2p2-new not doing anything because project=$project
fi
