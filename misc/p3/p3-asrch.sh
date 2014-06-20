#!/bin/sh
tmpdir=$1
xsl=/usr/local/oracc/lib/scripts
xsltproc $xsl/oas-SEARCH.xsl $tmpdir/search.xml >$tmpdir/search.txt
uni=`grep use-unicode $tmpdir/search.xml | grep true`
if [ "$uni" == "" ]; then
   unicode=
else
   unicode=-8
fi
echo asrch tmpdir=$tmpdir >&2
/usr/local/oracc/bin/se -s2 $unicode -x $tmpdir
#echo 'Content-type: text/xml; charset=utf-8'
#echo 
#xsltproc -stringparam tmpdir $tmpdir $xsl/oas-RETURN.xsl $tmpdir/search.xml \
#    | sed s,@TMPDIR@,$tmpdir,g \
#    | xmllint --xinclude -
#/usr/local/oracc/bin/p3-pager.plx asrch=yes project='#auto' tmpdir=$tmpdir list=$tmpdir/results.lst
