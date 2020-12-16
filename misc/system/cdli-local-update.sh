#!/bin/sh
# This script retrieves the CDLI serve data from build-oracc and installs it on a local machine.
# Steve uses it for his dev box.
cd $ORACC_BUILDS/srv
serve-install.sh cdli build-oracc.museum.upenn.edu
serve-index.sh cdli build-oracc.museum.upenn.edu
