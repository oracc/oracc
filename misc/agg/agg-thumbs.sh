#!/bin/sh
agg-test.sh || exit 1
mkdir -p ${ORACC}/agg/projects/images
cd ${ORACC}/agg/projects
for proj in `agg-list-public-projects.sh` ; do
    thumb=${ORACC}/$proj/00lib/thumb.png
    if [ -r $thumb ]; then
	d=`dirname $proj`
	mkdir -p ${ORACC}/www/agg/$d
	cp -fv $thumb ${ORACC}/www/agg/$proj.png
    else
	thumb2=${ORACC}/etc/projects/$proj/thumb.png
	if [ -r $thumb2 ]; then
	    d=`dirname $proj`
	    mkdir -p ${ORACC}/www/agg/$d
	    cp -fv $thumb2 ${ORACC}/www/agg/$proj.png
	else
	    echo agg-thumbs.sh: $proj has no thumbnail image $thumb
	fi
    fi
done
