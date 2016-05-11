#!/bin/sh
project=`oraccopt`
if [ "$project" == "" ]; then
    echo otf2tex.sh: must be run from a project directory
    exit 1
fi
driverpath=$1
if [ "$driverpath" == "" ]; then
    if [ -r 00lib/project.otf ]; then
	driverpath=00lib/project.otf
    else
	echo otf2tex.sh: no .otf file given on command line and no 00lib/project.otf
	exit 1
    fi
fi
keep=$2
driverdir=`dirname $driverpath`
drivername=`basename $driverpath`
driverbase=`basename $drivername .otf`
odtdir=01bld/$driverbase
rm -fr $odtdir ; mkdir -p $odtdir
echo ox -P$project -d$odtdir $driverpath
ox -P$project -d$odtdir $driverpath | xmllint --xinclude - | xsltproc - \
    | xsltproc ${ORACC}/lib/scripts/odt-table-width.xsl - \
    | xsltproc -stringparam package "$odtdir" ${ORACC}/lib/scripts/doc-split.xsl -
cwd=`pwd`; cd $odtdir
mkdir -p pictures ; odtpictures.plx >/dev/null
if [ -r $ORACC/$project/00lib/project-odtTeX.xsl ]; then
    odtTeX=$ORACC/$project/00lib/project-odtTeX.xsl
elif [ -r ~/00lib/project-odtTeX.xsl ]; then
    odtTeX=~/00lib/project-odtTeX.xsl
else
    odtTeX=$ORACC/lib/scripts/oracc-odtTeX.xsl
fi
echo otf2tex.sh: creating .tex output using $odtTeX
xsltproc -xinclude $odtTeX content.xml >$driverbase.tex
oracctex $driverbase 2>/dev/null
pdfdir=$ORACC/$project/00any/pdf
mkdir -p $pdfdir
mv $driverbase.pdf $pdfdir
mv $driverbase.log $pdfdir
cd $cwd
#if [ "$keep" == "" ]; then
#    rm -fr $odtdir
#fi
echo PDF output is in 00any/pdf/$driverbase.pdf
