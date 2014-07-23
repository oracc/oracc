#!/bin/sh
WWW=${ORACC}/www/doc/user
for  a in `find . -name '*.xdf'`; do \
    d=`dirname $a` ; \
    mkdir -p ${WWW}/$d ; \
    (cd $d ; xdfmanager.plx ; \
        cp -a html/* ${WWW}/$d ; \
    rm -fr html) \
done
../00tools/mk-index.plx -s <order.txt
../00tools/mk-index-html.sh user ${WWW}
