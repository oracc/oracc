#!/bin/sh
shopt -s nullglob

project=`oraccopt`

echo "o2-json.sh: generating JSON for $project"

rm -f 01bld/metadata.json

xsltproc ${ORACC}/lib/scripts/config-json.xsl 02xml/config.xml >01bld/config.json

(cd 01bld ;
 echo '{' >metadata.json
 echo '	"type": "metadata",' >>metadata.json
 cat config.json >>metadata.json
 for j in formats.json witnesses.json; do
     if [ -r $j ]; then
	 echo ',' >>metadata.json
	 cat $j >>metadata.json
     fi
 done
 echo >>metadata.json
 echo '}' >>metadata.json
 )

cp -f 01bld/metadata.json 01bld/catalogue.json 02www

if [ -s 01bld/lists/xtfindex.lst ]; then
    corpus-json.plx $project 01bld/lists/xtfindex.lst >02www/corpus.json
fi

if [ -r 02pub/cat/cat.dbi ]; then
    sedbg -p $project -i cat | index-json.plx $project cat >02www/index-cat.json
fi
if [ -r 02pub/txt/txt.dbi ]; then
    sedbg -p $project -i txt | index-json.plx $project txt >02www/index-txt.json
fi
if [ -r 02pub/tra/tra.dbi ]; then
    sedbg -p $project -i tra | index-json.plx $project tra >02www/index-tra.json
fi
if [ -r 02pub/lem/lem.dbi ]; then
    sedbg -p $project -i lem | index-json.plx $project lem >02www/index-lem.json
fi

for a in 02pub/cbd/* ; do 
    lang=`basename $a`
    cbd-json.plx ${project}:$lang >02www/gloss-$lang.json
    sort -u -o $a/mangle.tab $a/mangle.tab
    sedbg -p $project -i cbd/$lang | index-json.plx $project cbd/$lang >02www/index-$lang.json
done

(cd 02www ; touch manifest.json ;
    ls -1 *.json | o2-json.plx $project >manifest.json )

chmod 0644 02www/*.json
