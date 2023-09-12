#!/bin/sh
project=`oraccopt`

if [ -r 02pub/cat/cat.dbi ]; then
    echo Creating cat index
    sedbg -p $project -i cat | index-json.plx $project cat  >01bld/json/index-cat.json
fi
if [ -r 02pub/txt/txt.dbi ]; then
    echo Creating txt index
    sedbg -p $project -i txt | index-json.plx $project txt  >01bld/json/index-txt.json
fi
if [ -r 02pub/tra/tra.dbi ]; then
    echo Creating tra index
    sedbg -p $project -i tra | index-json.plx $project tra  >01bld/json/index-tra.json
fi
if [ -r 02pub/lem/lem.dbi ]; then
    echo Creating lem index
    sedbg -p $project -i lem | index-json.plx $project lem  >01bld/json/index-lem.json
fi

for a in 01bld/*/*.g2x ; do 
    lang=`basename $a .g2x`
    if [ -s $a ]; then
	if [[ $lang == qpn-* ]]; then
	    echo Skipping $lang--slice from main qpn glo instead
	else
	    echo Creating json for $project:$lang glossary
	    cbd-json.plx $project:$lang >01bld/json/gloss-$lang.json
	    mangletab=02pub/cbd/$lang/mangle.tab
	    dbi=02pub/cbd/$lang/cbd.dbi
	    if [ -r $dbi ]; then
		if [ -r $mangletab ]; then
		    echo Creating json for $lang glossary index
		    sort -u -o $mangletab $mangletab
		    sedbg -p $project -i cbd/$lang \
			| index-json.plx $project cbd/$lang \
					 >01bld/json/index-$lang.json
		fi
	    fi
	fi
    fi
done

echo ''
