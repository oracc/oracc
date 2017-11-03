#!/bin/sh
echo Rebuilding CDLI with cdli-rebuild.sh ...
(cd 00git ; git pull)
head -1 00git/cdli_catalogue_1of2.csv >00lib/fieldnames.row
00bin/csv-clean.plx
o2-corpus.sh
