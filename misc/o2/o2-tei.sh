#!/bin/sh
project=`oraccopt`;

if [ "$project" == "" ]; then
    echo o2-tei.sh: must run in a project or subproject
    exit 1
fi

tei_list=`oraccopt . build-tei-list`;
tei_trim=`oraccopt . build-tei-trim`;
tei_dist=`oraccopt . build-tei-dist`;
#tei_xml=
#tei_list_file=
#xmlfile=

if [ "$tei_list" == "" ]; then
    # nothing to do
    #    exit 0;
    tei_list=all
fi
if [ "$tei_list" == "atf" ]; then
    tei_list_file=01bld/lists/have-atf.lst
elif [ "$tei_list" == "master" ]; then
#    tei_list_file=01bld/lists/master.lst
    tei_list_file=02pub/cat/pqids.lst
elif [ "$tei_list" == "lem" ]; then
    tei_list_file=01bld/lists/lemindex.lst
elif [ "$tei_list" == "all" ]; then
    tei_list_file=01bld/lists/xtfindex.lst
elif [ "$tei_list" != "" ]; then
    echo "o2-tei.sh: value '$tei_list' invalid for option 'build-tei-list'"
    exit 1
fi

tei-project.sh $project $tei_list_file

if [ "$tei_trim" == "yes" ]; then
    xmlfile=`tei-trim.sh $project`
else
    xmlfile=`tei-no-trim.sh $project`
fi

# schema not good right now? check on unix box
#rnv $ORACC/lib/schemas/xtf-tei.rnc 01tmp/$xmlfile 2>01tmp/tei-valid.log
#wc -l 01tmp/tei-valid.log

#if [ "$tei_dist" == "yes" ]; then
tei-dist.sh $xmlfile
#fi
o2-index-downloads.sh
