#!/bin/sh
echo "o2-json.sh: generating JSON for $project"

cp 01bld/metadata.json 02www

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
