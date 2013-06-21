#!/bin/sh
project=`oraccopt`
p3-project-data.plx
xsltproc --stringparam project $project \
    ${ORACC}/lib/scripts/p3-project.xsl $ORACC/lib/data/p3-template.xml | \
    sed -e "s%@@PROJECT@@%$project%" >02pub/p3.html
p3-pager.plx noheader=1 project=epsd2 >02www/p3init.html
sed -e "s%@@PROJECT@@%$project%" < $ORACC/lib/data/as-base.xml >02www/as.xml
