#!/bin/sh
if [ "$1" == "-s" ]; then
    STDOUT=$1;
    shift;
fi
if ! [ "$1" == "" ]; then
    ODS2ATF=$1 ; \
fi
if [ "$ODS2ATF" == "" ]; then
    echo "usage: ods2atf.sh [-s] [FILE]"
    echo " If [FILE] is not given, ODS2ATF should be set in environment"
    echo " If -s is given, output is to stdout"
    exit 1
fi
rm -f ${ODS2ATF}.atf
if [ "$STDOUT" == "-s" ]; then
    unzip -p ${ODS2ATF} content.xml | \
	xsltproc ${ORACC}/lib/scripts/ods2atf.xsl - 2>ods2atf.log
else
    unzip -p ${ODS2ATF} content.xml | \
	xsltproc ${ORACC}/lib/scripts/ods2atf.xsl - \
	>${ODS2ATF}.atf 2>${ODS2ATF}.log
fi
chmod -f -w ${ODS2ATF}.atf
