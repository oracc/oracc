#!/bin/sh
o2-prepare.sh
buildpolicy=`oraccopt . build-approved-policy`;
if [ "$buildpolicy" != "search" ]; then
    o2-glo.sh
fi
o2-xtf.sh $*
formats-json.plx >>01bld/metadata.json
echo '}' >>01bld/metadata.json
#o2-web.sh
o2-web-corpus.sh
#o2-prm.sh
o2-weblive.sh
