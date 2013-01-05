#!/bin/sh
oraccid=`id -u oracc`
if [ "$EUID" != "$oraccid" ]; then
    echo aggregation can only be done by the 'oracc' user
    exit 1
fi
cd ~/agg/projects
mkdir -p images
xsl=${ORACC}/lib/scripts
for img in `xsltproc $xsl/agg-public-image.xsl public-projects.xml` ; do
    project=`echo $img | sed 's/:.*$//'`
    image=`echo $img | sed 's/^.*://'`
    mkdir -p images/`dirname $project`
    thumb=`echo -n $image | sed s/\\\./-thumb./`
    if [ -e ${ORACC}/www/$project/images/$thumb ]; then
	cp -a ${ORACC}/www/$project/images/$thumb images/$project.png
    elif [ -e ${ORACC}/www/$project/images/$image ]; then
	convert -geometry '88' ${ORACC}/www/$project/images/$image images/$project.png
    elif [ -e ${ORACC}/etc/projects/$project/$image ]; then
	convert -geometry '88' ${ORACC}/etc/projects/$project/$image images/$project.png
    else
	echo agg-thumbs.sh: $project has no image file $image
    fi
done
