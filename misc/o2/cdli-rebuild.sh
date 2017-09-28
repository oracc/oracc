#!/bin/sh
echo Rebuilding CDLI with cdli-rebuild.sh ...
(cd 00git ; git pull)
(cd 00cat ; unzip ../00git/cdli_catalogue.csv.zip)
(cd 00src ; unzip ../00git/cdliatf_unblocked.atf.zip)
head -1 00cat/cdli_catalogue.csv >00lib/fieldnames.row
00bin/split-atf.plx
o2-corpus.sh
