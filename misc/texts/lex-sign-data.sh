#!/bin/sh
projects=$*
if [ "$projects" == "" ]; then
    echo 'lex-sign-data.sh: must give project(s) on command line'
    exit 1
fi
rm -fr 01tmp/lex/sign
mkdir -p 01tmp/lex/sign
for p in $* ; do
    withlem=$ORACC_BUILDS/$p/01bld/lists/withlem
    for a in `pqxpand xtf <$withlem | xargs grep -l -F 'subtype="sg"'`; do
       	lex-sign-xtf.sh $a
    done
done
lex-master.sh sign
lex-sign-data-2.sh
