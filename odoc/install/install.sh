#!/bin/sh
WWW=${ORACC}/www/doc/install
for  a in `find . -name '*.xdf'`; do \
    d=`dirname $a` ; \
    echo processing $d ; \
    mkdir -p ${WWW}/$d ; \
    (cd $d ; rm -fr html ; xdfmanager.plx ; \
        cp -af html/* ${WWW}/$d ; \
    [ -e etc ] && cp -af etc ${WWW}/$d ; ) \
done
cp -af ports/*.txt ${WWW}/ports
cp -af index.html ${WWW}
chmod -R o+r ${WWW}
