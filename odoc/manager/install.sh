#!/bin/sh
mkdir -p ${ORACC}/www/jemimap
cp -pR projconfig/jemimap/* ${ORACC}/www/jemimap
chmod -R o+r ${ORACC}/www/jemimap
WWW=${ORACC}/www/doc/manager
for  a in `find . -name '*.xdf'`; do \
    d=`dirname $a` ; \
    mkdir -p ${WWW}/$d ; \
    (cd $d ; xdfmanager.plx ; \
        cp -a html/* ${WWW}/$d ; \
    rm -fr html) \
done
../00tools/mk-index.plx -s <order.txt
../00tools/mk-index-html.sh manager ${WWW}
