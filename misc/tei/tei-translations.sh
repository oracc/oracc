#!/bin/sh
if [ ! -e 00lib/config.xml ]; then
    echo tei-project.sh: must run from a project or subproject directory
    exit 1
fi
if [ "$1" = "" ];
then
    echo tei-project.sh: must give project name as argument
    exit 1
fi
project=$1
if [ "$2" != "" ];
then
    teilist=$2
else
    teilist=01bld/lists/have-atf.lst
fi
SAXONJAR=$ORACC/lib/java/saxon.jar
XSL=$ORACC/lib/scripts
outdir=01tmp/teixtr
rm -fr $outdir ; mkdir -p $outdir
shopt -s nullglob
echo Creating TEI translations for texts listed in $teilist
cat $teilist | while read line; do
    pqx=`/bin/echo -n $line | cut -d'@' -f1 | sed 's/^.*://'`
    prj=`/bin/echo -n $line | sed 's/:.*$//'`
    xtfbase=${ORACC}/bld/$prj/${pqx:0:4}/$pqx/$pqx
    /bin/echo -n '<translations>' >$outdir/$pqx-xtr.xml
    for a in ${xtfbase}*.xtr ; do
	java -jar $SAXONJAR $a $XSL/xtr2tei.xslt project=$project | sed 's/\&amp;amp;/&amp;/g' >>$outdir/$pqx-xtr.xml
    done
    echo -n '</translations>' >>$outdir/$pqx-xtr.xml
done
