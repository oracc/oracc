#!/bin/sh
project=`oraccopt`

if [ -r 02pub/cat/cat.dbi ]; then
    echo cat index
    sedbg -p $project -i cat | index-json.plx $project cat  >01bld/json/index-cat.json
fi
if [ -r 02pub/txt/txt.dbi ]; then
    echo txt index
    sedbg -p $project -i txt | index-json.plx $project txt  >01bld/json/index-txt.json
fi
if [ -r 02pub/tra/tra.dbi ]; then
    echo tra index
    sedbg -p $project -i tra | index-json.plx $project tra  >01bld/json/index-tra.json
fi
if [ -r 02pub/lem/lem.dbi ]; then
    echo lem index
    sedbg -p $project -i lem | index-json.plx $project lem  >01bld/json/index-lem.json
fi

for a in 02pub/cbd/* ; do 
    lang=`basename $a`
    if [ -r 02pub/cbd/$lang/cbd.dbi ]; then
	echo $lang index
	cbd-json.plx ${project}:$lang >01bld/json/gloss-$lang.json
	sort -u -o $a/mangle.tab $a/mangle.tab
	sedbg -p $project -i cbd/$lang | index-json.plx $project cbd/$lang  >01bld/json/index-$lang.json
    fi
done
