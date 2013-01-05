#!/bin/sh
project=`oraccopt`
p2-project-data.plx
xsltproc --stringparam project $project \
    ${ORACC}/lib/scripts/p2-project.xsl $ORACC/lib/data/p2-base.xml | \
    sed -e "s%@@PROJECT@@%$project%" >02xml/p2.xml
sed -e "s%@@PROJECT@@%$project%" < $ORACC/lib/data/as-base.xml >02xml/as.xml
