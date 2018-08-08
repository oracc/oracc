#!/bin/sh
rm -fr 01tmp/lex/word
mkdir -p 01tmp/lex/word
for p in $* ; do
    withlem=$ORACC_BUILDS/$p/01bld/lists/withlem
    #    for a in `pqxpand xtf <$withlem | xargs grep -l -F 'subtype="wp"'`; do
    for a in `pqxpand xtf <$withlem`; do
       	lex-word-xtf.sh $a
    done
done
