#!/bin/sh
shopt -s nullglob
project=`oraccopt`

sort -u -o 02pub/cat/mangle.tab 02pub/cat/mangle.tab
sort -u -o 02pub/txt/mangle.tab 02pub/txt/mangle.tab
sort -u -o 02pub/tra/mangle.tab 02pub/tra/mangle.tab
sort -u -o 02pub/lem/mangle.tab 02pub/lem/mangle.tab

sedbg -p $project -i cat | cbd-json.plx $project cat >02www/index-cat.json
sedbg -p $project -i txt | cbd-json.plx $project txt >02www/index-txt.json
sedbg -p $project -i tra | cbd-json.plx $project tra >02www/index-tra.json
sedbg -p $project -i lem | cbd-json.plx $project lem >02www/index-lem.json

for a in 02pub/cbd/* ; do 
    lang=`basename $a`
    secbdx -p $project -l $lang 01bld/$lang/articles.xml
    cp 01bld/$lang/letter_ids.tab $a
    sort -u -o $a/mangle.tab $a/mangle.tab
    sedbg -p $project -i cbd/$lang | cbd-json.plx $project cbd/$lang >02www/index-$lang.json
done
