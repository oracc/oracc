#!/bin/sh
agg-test.sh || exit 1
outbase=$ORACC/agg/master-linkbase.xml
echo '<linkbase>' >$outbase
for a in `agg-list-public-projects.sh` ; do
    if [ -r $ORACC/bld/$a/linkbase.xml ]; then
	echo '<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"><xsl:output method="xml" encoding="utf-8" omit-xml-declaration="yes"/><xsl:template match="/"><xsl:copy-of select="linkbase/project"/></xsl:template></xsl:stylesheet>' \
	    | xsltproc - $ORACC/bld/$a/linkbase.xml >>$outbase
    fi
done
echo '</linkbase>' >>$outbase
