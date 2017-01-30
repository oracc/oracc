#!/bin/sh

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

sedbg -p $project -i cat | cbd-json.plx $project cat >02www/index-cat.json
sedbg -p $project -i txt | cbd-json.plx $project txt >02www/index-txt.json
sedbg -p $project -i tra | cbd-json.plx $project tra >02www/index-tra.json
sedbg -p $project -i lem | cbd-json.plx $project lem >02www/index-lem.json

for a in 02pub/cbd/* ; do 
    lang=`basename $a`
    sort -u -o $a/mangle.tab $a/mangle.tab
    sedbg -p $project -i cbd/$lang | cbd-json.plx $project cbd/$lang >02www/index-$lang.json
done

(cd 02www ; touch manifest.json ;
    ls -1 *.json | o2-json.plx $project >manifest.json )

chmod 0644 02www/*.json
