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
u=`uname`
if [ "$uname" = "Darwin" ]; then
    COMPRESS=-y
else
    COMPRESS=-j
fi
zip=request.tar.gz ; export zip
ziplist=01tmp/request-files.lst
rm -f $zip
rm -f $ziplist
# COPYFILE_DISABLE suppresses addition of ._* files under Mac OS X at least up
# to 10.8
COPYFILE_DISABLE=1 ; export COPYFILE_DISABLE
find 00atf 00bib 00cat 00lib 00web -type f \
    -name '*.atf' \
    -or -name '*.css' \
    -or -name '*.glo' \
    -or -name '*.html' \
    -or -name '*.ico' \
    -or -name '*.js' \
    -or -name '*.lst' \
    -or -name '*.ods' \
    -or -name '*.xml' \
    >$ziplist
if [ -r 00lib/p3colours.txt ]; then
    echo 00lib/p3colours.txt >>$ziplist
fi
if [ -r 00lib/thumb.png ]; then
    echo 00lib/thumb.png >>$ziplist
fi
tar $COMPRESS -cf $zip -T $ziplist
export version zip
