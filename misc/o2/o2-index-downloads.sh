#!/bin/sh
mkdir -p 02www/downloads
index-downloads.plx >02www/downloads/index.html
chmod o+r 02www/downloads/index.html
