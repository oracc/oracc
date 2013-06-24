#!/bin/sh
agg-test.sh || exit 1
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
