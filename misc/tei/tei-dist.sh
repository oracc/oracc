#!/bin/sh
mkdir -p 02www/downloads
zip=`basename $1 .xml`
zip -j 01tmp/$zip.zip 01tmp/$1
mv -f 01tmp/$zip.zip 02www/downloads/
chmod o+r 02www/downloads/$zip.zip
echo TEI distribution now available at project URL downloads/$zip.zip
