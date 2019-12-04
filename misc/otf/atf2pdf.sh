#!/bin/sh
#
# N.B. THIS SCRIPT IS TO MAKE A PDF FROM A SINGLE ATF FILE
#
qname=$1
if [ "$qname" == "" ]; then
    echo atf2pdf.sh: must give PROJECT::PXQID on commandline
    exit 1
fi    
project=`oraccopt`
if [ "$project" == "" ]; then
    echo atf2pdf.sh: must be run from a project directory
    exit 1
fi
atffile=`pqxpand atf $qname`
if [ ! -r $atffile ]; then
    echo atf2pdf.sh: ATF file $atffile non-existent or unreadable
    exit 1
fi

pqxbase=`basename $atffile .atf`
workdir=`dirname $atffile`
pdffile="$pqxbase.pdf"
makefile=$ORACC_BUILDS/lib/data/OMakefile-pdf
echo Making $pqxbase.pdf in $workdir using $makefile
(cd $workdir ; PROJECT=$project make -f $makefile $pdffile)

exit 0
