#!/bin/sh
PROG=oracc-client-pack-project.sh
if [ ! -f 00lib/config.xml ]; then
    echo $PROG: 'must be run in an Oracc directory (no 00lib/config.xml). Stop.'
    exit 1
fi
version=`cat .version`
if [ "$version" = "" ]; then
    echo $PROG: no .version file--run oracc version
    version=000
fi
zip=request.tar.gz ; export zip
rm -f $zip
# COPYFILE_DISABLE suppresses addition of ._* files under Mac OS X at least up
# to 10.8
COPYFILE_DISABLE=1 ; export COPYFILE_DISABLE
find 00atf 00lib 00web -type f \
    -name '*.atf' \
    -or -name '*.css' \
    -or -name '*.glo' \
    -or -name '*.html' \
    -or -name '*.js' \
    -or -name '*.lst' \
    -or -name '*.xml' \
    | tar cyf $zip -T -
export version zip
