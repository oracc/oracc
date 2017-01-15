#!/bin/sh
bld=$ORACC_HOME/$project/01bld
if [ -d $bld/est]; then
    cd $ORACC_HOME/$project/02www
    rm -fr estindex
    ls -1 $bld/est/project.est >$bld/estindex.lst
    grep -l -m1 -r 'generator. content=.Oracc ESP' $bld/espdev >>$bld/estindex.lst
    find $bld/est -type f >>$bld/estindex.lst
    cat $bld/estindex.lst | estcmd gather -tr -ic UTF-8 estindex - >$bld/estindex.log
    chmod -R o+r estindex
fi

