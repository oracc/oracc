#!/bin/sh
#
# N.B. THIS SCRIPT IS TO MAKE A PDF FROM A SINGLE ATF FILE
#
qname=$1
if [ "$qname" == "" ]; then
    echo atf2pdf.sh: must give PROJECT:PXQID on commandline
    exit 1
fi

atffile=`/Users/stinney/orc/bin/pqxpand atf $qname`

if [ ! -r $atffile ]; then
    echo atf2pdf.sh: ATF file $atffile non-existent or unreadable
    exit 1
fi

project=`/bin/echo -n $qname | cut -d: -f1` ; export project
PATH=$PATH:/Users/stinney/orc/bin ; export PATH
workdir=`dirname $atffile`
pqxbase=`basename $atffile .atf`
pdffile="$pqxbase.pdf"
makefile=/Users/stinney/orc/lib/data/OMakefile-pdf
>&2 echo Making $project $pqxbase.pdf in $workdir using $makefile 
(cd $workdir ; rm -fr odt ; mkdir -p odt ; make -s -f $makefile $pdffile)

exit 0
