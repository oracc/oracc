#!/bin/sh
project=$1
if [ "$project" == "" ]; then
    echo "srv-project.sh: must give project as argument"
    exit 1
fi
if [ ! -d ${ORACC}/$project ]; then
    echo "srv-project.sh: $project is not a project in Oracc dir ${ORACC}"
    exit 1
fi
projfile=`/bin/echo -n $project | tr / -`

tarball=${ORACC}/www/downloads/builds/${projfile}-build.tar.gz

echo "project-for-builds.sh: generating $tarball"

cd ${ORACC}
rm -f $tarball

cd $project
subs=""
shopt -s nullglob
for a in [a-z][a-z0-9][a-z0-9][a-z0-9]*; do
    subs="$subs --exclude $a"
done
cd $ORACC

tar --exclude 00any --exclude 00res --exclude estindex --exclude espdev \
    $subs \
    -zcf $tarball $project/00* {bld,pub,xml,www}/$project
