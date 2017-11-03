#!/bin/sh
echo Rebuilding CDLI with cdli-rebuild.sh ...
(cd 00git ; git pull)
#(cd 00cat ; unzip -o ../00git/cdli_catalogue.csv.zip)
#(cd 00src ; unzip -o ../00git/cdliatf_unblocked.atf.zip)
head -1 00git/cdli_catalogue_1of2.csv >00lib/fieldnames.row
#00bin/split-atf.plx
#o2-catalog.sh
o2-corpus.sh
