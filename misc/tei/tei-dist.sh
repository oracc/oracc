#!/bin/sh
mkdir -p 02www/downloads
zip -j 01tmp/teiCorpus.zip 01tmp/$1
mv -f 01tmp/teiCorpus.zip 02www/downloads/
chmod o+r 02www/downloads/teiCorpus.zip
