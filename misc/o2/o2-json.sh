#!/bin/sh
shopt -s nullglob

project=`oraccopt`

echo "o2-json.sh: generating JSON for $project"

echo "o2-json.sh: metadata ..."

rm -f 01bld/metadata.json

xsltproc ${ORACC}/lib/scripts/config-json.xsl 02xml/config.xml >01bld/config.json

metadata-json.sh $project

cat 01bld/metadata.json  >02www/metadata.json
cat 01bld/catalogue.json  >02www/catalogue.json

echo "o2-json.sh: corpus ..."

if [ -s 01bld/lists/xtfindex.lst ]; then
    corpus-json.plx $project 01bld/lists/xtfindex.lst  >02www/corpus.json
fi

echo "o2-json.sh: core indexes ..."

if [ -r 02pub/cat/cat.dbi ]; then
    sedbg -p $project -i cat | index-json.plx $project cat  >02www/index-cat.json
fi
if [ -r 02pub/txt/txt.dbi ]; then
    sedbg -p $project -i txt | index-json.plx $project txt  >02www/index-txt.json
fi
if [ -r 02pub/tra/tra.dbi ]; then
    sedbg -p $project -i tra | index-json.plx $project tra  >02www/index-tra.json
fi
if [ -r 02pub/lem/lem.dbi ]; then
    sedbg -p $project -i lem | index-json.plx $project lem  >02www/index-lem.json
fi

echo "o2-json.sh: language glossaries and indexes ..."

for a in 02pub/cbd/* ; do 
    lang=`basename $a`
    echo $lang
    cbd-json.plx ${project}:$lang >02www/gloss-$lang.json
    sort -u -o $a/mangle.tab $a/mangle.tab
    sedbg -p $project -i cbd/$lang | index-json.plx $project cbd/$lang  >02www/index-$lang.json
done

echo "o2-json.sh: manifest ..."

(cd 02www ; touch manifest.json ;
 ls -1 *.json | o2-json.plx $project  >manifest.json ; \
     rm -f json-pre-jq.zip ; zip -q -r json-pre-jq.zip *.json ; \
     json-licensify.plx *.json ; \
     rm -f json.zip ; zip -q -r json.zip *.json corpusjson ; \
     chmod 0644 02www/*.json 02www/json.zip
)

echo "o2-json.sh: done ..."
