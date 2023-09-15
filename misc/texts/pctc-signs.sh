#!/bin/sh
if [ ! -d 01tmp ]; then
    echo "$0: I only work when there is an 01tmp directory. Stop."
    exit 1
fi
libscripts=$ORACC_BUILDS/lib/scripts
echo "$0: timing oxx ..."
time oxx -l01tmp/pctc.log 00atf/*.atf >01tmp/corpus.xml
echo "$0: timing word-children.xsl ..."
time xsltproc $libscripts/word-children.xsl 01tmp/corpus.xml | sort | sed 's/^/pctc:/' >01tmp/w-k.tab
cut -f2 01tmp/w-k.tab | gdlx -s -p pctc | cut -f1 >01tmp/oid.col
cut -f1 01tmp/w-k.tab | paste - 01tmp/oid.col > 01tmp/w-o.tab
concord -twcl* <01tmp/w-o.tab >01tmp/o-f-w.tab
pctc-xis.plx <01tmp/o-f-w.tab
pctc-divs.sh
(cd 01tmp/xis ;
 for a in o* ; do
     if [ -d $a ]; then
	 (cd $a ;
	  echo pctc-xis-data.sh $a ...
	  pctc-xis-data.sh )
     fi
 done
)
