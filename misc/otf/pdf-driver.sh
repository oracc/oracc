#!/bin/sh
#
# N.B. THIS SCRIPT IS TO MAKE A PDF FROM otf-driver.plx OUTPUT
#
project=$1
pqxname=$2
otfname="$pqxname.otf"
texname="$pqxname.tex"
pdfname="$pqxname.pdf"

translang=$ORACC_TRANS_DEFAULT_LANG
if [ "$translang" = "" ]; then
    argtranslang=""
else
    argtranslang="-7$translang"
fi

ox $argtranslang -p $project $otfname | xmllint --xinclude - | xsltproc - \
    | xsltproc ${ORACC}/lib/scripts/odt-table-width.xsl - \
    | xsltproc -stringparam package odt ${ORACC}/lib/scripts/doc-split.xsl -
mv odtpictures.lst odt
(cd odt
 mkdir -p pictures ; odtpictures.plx >/dev/null
 projbase=`/bin/echo -n $project | sed 's#/.*$##'`
 if [ -r $ORACC_BUILDS/$project/00lib/project-odtTeX.xsl ]; then
     odtTeX=$ORACC_BUILDS/$project/00lib/project-odtTeX.xsl
 elif [ -r $ORACC_BUILDS/$projbase/00lib/project-odtTeX.xsl ]; then
     odtTeX=$ORACC_BUILDS/$projbase/00lib/project-odtTeX.xsl
 else
     odtTeX=$ORACC/lib/scripts/oracc-odtTeX.xsl
 fi
 odtTeX=$ORACC_BUILDS/rinap/00lib/project-odtTeX.xsl
 echo pdf-driver.sh: creating $texname output using $odtTeX
 xsltproc -xinclude $odtTeX content.xml >$texname
 oracctex $texname 2>/dev/null
 mv $pdfname ..
)
