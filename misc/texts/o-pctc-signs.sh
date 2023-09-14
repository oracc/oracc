#!/bin/sh
if [ ! -d 01tmp ]; then
    echo "$0: I only work when there is an 01tmp directory. Stop."
    exit 1
fi
libscripts=$ORACC_BUILDS/lib/scripts
echo "$0: timing oxx ..."
time oxx -l01tmp/pctc.log 00atf/*.atf >01tmp/corpus.xml
echo "$0: timing word-children.xsl ..."
time xsltproc $libscripts/word-children.xsl 01tmp/corpus.xml | sort | sed 's/^/pctc:/' >01tmp/w.tab
echo 'sign	oid' >01tmp/w-oid.tab
cut -f2 01tmp/w.tab | sort -u | slx -r -p pctc -n pctc >>01tmp/w-oid.tab
rocox -f -x sign2oids <01tmp/w-oid.tab >01tmp/w-oid.xml
echo "$0: timing concord ..."
time concord -xwcl* <01tmp/w.tab >01tmp/w.xml
xsltproc $libscripts/pctc-add-oids.xsl 01tmp/w.xml >02xml/sign-instances.xml
