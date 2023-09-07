#!/bin/sh
oxx 00atf/*.atf | xsltproc word-children.xsl - >w.tab
echo 'sign	oid' >w-oid.tab
cut -f2 w.tab | sort -u | slx -r -p pctc -n pctc >>w-oid.tab
rocox -x w-oid.tab >w-oid.xml
concord -xwcl* w.tab >w.xml
xsltproc word-oids.xsl w.xml >w-with-oids.xml
