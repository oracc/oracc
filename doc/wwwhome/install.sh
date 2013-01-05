#!/bin/sh
if [ "$ORACC" = "" ];
then
    echo wwwhome/install.sh: must set ORACC environment variable first
    exit 1
fi
agg-project-list.sh
WWW=${ORACC}/www
for a in *.html *.css; do \
    rm -f ${WWW}/$a ; \
done
mkdir -p ${WWW}/{css,util}
(cd util ; cp -f * ${WWW}/util ; cd ${WWW}/util ; chmod o+r . *)
cp -f *.html *.js *.png *.ico ${WWW}
chmod -w ${WWW}/*.html ${WWW}/*.png ${WWW}/*.css ${WWW}/*.js
chmod o+r ${WWW}/*.html ${WWW}/*.{ico,png,css,js}
cp -f *.css ${WWW}
chmod o+r ${WWW}/*.css
