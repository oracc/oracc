#!/bin/sh
if [ ! -d 01tmp ]; then
    echo "$0: I only work when there is an 01tmp directory. Stop."
    exit 1
fi
libscripts=$ORACC_BUILDS/lib/scripts
oxx 00atf/*.atf | xsltproc $libscripts/word-children.xsl - >01tmp/w.tab
echo 'sign	oid' >01tmp/w-oid.tab
cut -f2 01tmp/w.tab | sort -u | slx -r -p pctc -n pctc >>01tmp/w-oid.tab
rocox -f -x sign2oids <01tmp/w-oid.tab >01tmp/w-oid.xml
concord -xwcl* <01tmp/w.tab >01tmp/w.xml
xsltproc $libscripts/pctc-add-oids.xsl 01tmp/w.xml >01tmp/w-with-oids.xml
