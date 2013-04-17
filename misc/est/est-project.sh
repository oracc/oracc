#!/bin/sh
project=`oraccopt`
if [ "$project" == "" ]; then
    echo est-project.sh: must be called in a project directory
    exit 1
fi
rm -fr 01bld/est
mkdir -p 01bld/est
est-projuri.sh 2>01tmp/est-projuri.log
est-corpus.sh 2>01tmp/est-corpus.log
est-glo.sh 2>01tmp/est-glo.log
bld=$ORACC_HOME/$project/01bld
tmp=$ORACC_HOME/$project/01tmp
cd $ORACC_HOME/$project/02www
rm -fr estindex
ls -1 $bld/est/project.est >$tmp/estindex.lst
grep -l -m1 -r 'generator. content=.Oracc ESP' * >>$tmp/estindex.lst
#find cbd -name '*.*.html' | grep -v qpn-x- >>$tmp/estindex.lst
find $bld/est -type f >>$tmp/estindex.lst
cat $tmp/estindex.lst | estcmd gather -tr -ic UTF-8 estindex - >$tmp/estindex.log
chmod -R o+r estindex
