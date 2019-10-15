#!/bin/sh
project=$1
xml=$2
xis=$3
lib=$ORACC_BUILDS/lib/scripts
xsltproc $lib/bki-subset.xsl $xml >bki-subset.xml
xsltproc -stringparam xis $xis $lib/bki-xis.xsl bki-subset.xml >bki-xis.xml
xsltproc $lib/bki-xis-group.xsl bki-xis.xml \
	 | xsltproc $lib/bki-labels.xsl - >bki-grouped.xml
r-labels.plx $project bki-grouped.xml
xsltproc $lib/bki-sort.xsl bki-grouped+.xml >bki-sorted.xml
r-uniq.plx bki-sorted.xml
r-trim-label2.plx bki-sorted+.xml
r-vol.plx bki-sorted++.xml
