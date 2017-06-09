#!/bin/sh

JSON=`oraccopt . json`

if [ "$JSON" == "no" ]; then
    echo JSON=no
    exit 0
fi

shopt -s nullglob
project=`oraccopt`

if [ "$project" == "" ]; then
    echo o2-json.sh: must be run from a project directory. Stop.
    exit
fi

function has_jsonable {
    if [ -s 01bld/cdlicat.xmd ]; 
	then return 1
    fi
    if [ -s 01bld/lists/have-xtf.lst ]; 
	then return 1
    fi
    if [ -s 01bld/lists/proxy-atf.lst ]; 
	then return 1
    fi
    for glo in 00lib/*.glo ; do
	if [ -s $glo ]; 
	then return 1
	fi
    done
    return 0
}

has_jsonable 
if [ $? == 0 ]
then
    echo "o2-json.sh: $project has no jsonable files. Stop."
    exit 0
fi

(cd $ORACC_BUILDS/jsn; mkdir -p $project; cd $project; rm -fr *)
(cd 01bld; rm -fr json; ln -sf $ORACC_BUILDS/jsn/$project json)

jsonlog=01tmp/json.log

echo "o2-json.sh: generating JSON for $project"

if [ -r 01bld/cat.geojson ]; then
    cp 01bld/cat.geojson 01bld/json/
fi

if [ -r 01bld/catalogue.json ]; then
    cp -f 01bld/catalogue.json 01bld/json/
fi

echo "o2-json.sh: metadata ..."
metadata-json.sh

echo "o2-json.sh: corpus ..."
corpus-json.plx >$jsonlog 2>&1

echo "o2-json.sh: indexes ..."
index-json.sh >>$jsonlog 2>&1

echo "o2-json.sh: validating and adding licensing ..."
validate-json.sh >>$jsonlog 2>&1

errors-json.plx
if [ -r 01tmp/json-error.log ];
then
    echo "o2-json.sh: internal errors in JSON processing; please tell Steve."
    echo "o2-json.sh: skipping zipping JSON."
else
    echo "o2-json.sh: zipping json ..."
    zip=`zip-json.sh`
    rm -fr 01bld/json/*
    echo "o2-json.sh: JSON created and validated without errors."
    echo "o2-json.sh: $zip is now public"
fi
