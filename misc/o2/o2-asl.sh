#!/bin/sh

libscripts=$ORACC_BUILDS/lib/scripts
project=`oraccopt`
slname=`oraccopt . asl-signlist`
if [[ "$slname" == "" ]]; then
    slname=project.asl
fi

if [ -r 00lib/$slname ]; then
    mkdir -p 02pub/sl
    #sx -s 00lib/$slname >02pub/sl/sl.tsv
    if [ $? != 0 ]; then
       	exit
    fi
    slix -v
    sx -x 00lib/$slname >02xml/sl.xml
    if [[ "$project" == "pctc" ]]; then
	echo "$0: indexing signs in PCTC"
	pctc-signs.sh
    fi
#    if [ -r 02xml/sign-instances.xml ]; then
#	xsltproc $libscripts/sxweb-add-icounts.xsl 02xml/sl.xml >tmp.xml
#	mv tmp.xml 02xml/sl.xml
#	rm -fr 01bld/tislists
#	xsltproc $libscripts/sxweb-tis.xsl 02xml/sign-instances.xml
#   fi
    chmod -R o+r 02pub/sl 02xml/sl.xml
    sxweb.sh
fi
