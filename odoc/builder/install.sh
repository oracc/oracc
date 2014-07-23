#!/bin/sh
WWW=${ORACC}/www/doc/builder
for  a in `find . -name '*.xdf'`; do \
    d=`dirname $a` ; \
    mkdir -p ${WWW}/$d ; \
    (cd $d ; xdfmanager.plx ; \
        cp -a html/* ${WWW}/$d ; \
    [ -e etc ] && cp -af etc ${WWW}/$d ; rm -fr html) \
done
../00tools/mk-index.plx -s <order.txt
../00tools/mk-index-html.sh builder ${WWW}
cp dates/dsa.xml ${ORACC}/lib/data ; chmod o+r ${ORACC}/lib/data/dsa.xml