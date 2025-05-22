#!/bin/sh
mkdir -p 02www/downloads
odo-chmod.sh g+w 02www/downloads
index-downloads.plx >02www/downloads/index.html
odo-chmod.sh o+r 02www/downloads/index.html
