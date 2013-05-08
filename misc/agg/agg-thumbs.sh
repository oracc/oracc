#!/bin/sh

if [ "$ORACC_MODE" != "single" ]; then
    oraccid=`id -u oracc`
    if [ "$EUID" != "$oraccid" ]; then
	echo aggregation can only be done by the 'oracc' user
	exit 1
    fi
fi
mkdir -p ${ORACC}/agg/projects/images
cd ${ORACC}/agg/projects
for proj in `agg-list-public-projects.sh` ; do
    subp=`basename $proj`
    thumb=$subp-thumb.png
    if [ -e ${ORACC}/www/$proj/$thumb ]; then
	echo proj=$proj thumb=$thumb
	cp -fv ${ORACC}/www/$proj/$thumb ${ORACC}/www/agg/$proj.png
    else
	echo agg-thumbs.sh: $proj has no thumbnail image $thumb
    fi
done
