#!/bin/sh
project=$1
tarball=$2
if [ "$project" == "" ]; then
    echo "serve-install-sub.sh: must give project as first arg. Stop."
    exit 1
fi
if [ "$tarball" == "" ]; then
    echo "serve-install-sub.sh: must give tarball as second arg. Stop."
    exit 1
fi
if [ -r $tarball ]; then
    true
else
    echo "serve-install-sub.sh: no such tarball $tarball. Stop."
    exit 1
fi
tarproj=`basename $tarball .tar.gz | tr - /`
if [ "$tarproj" == "$project" ]; then
    true
else
    echo "serve-install-sub.sh: tarball project $tarproj != project $project. Stop."
    exit 1
fi
echo "Installing $tarball"
cd $ORACC_BUILDS
sudo tar zxof $tarball
cd $project
rm -f 01bld 02pub 02www 02xml
ln -sf $ORACC_BUILDS/bld/$project 01bld
ln -sf $ORACC_BUILDS/pub/$project 02pub
ln -sf $ORACC_BUILDS/www/$project 02www
ln -sf $ORACC_BUILDS/xml/$project 02xml
