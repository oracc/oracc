#!/bin/sh
if ! [ "$1" == "" ]; then
    ODS2ATF=$1 ; \
fi
(cd `dirname $ODS2ATF` ; \
    unzip -p ${ODS2ATF} content.xml >orig-content.xml ; \
    xsltproc -xinclude ${ORACC}/lib/scripts/ods-lem.xsl \
    ${ODS2ATF}.lem >content.xml ; \
    zip -q ${ODS2ATF} content.xml )
