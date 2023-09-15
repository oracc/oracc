#!/bin/sh
sloid.sh pctc | pctc-count-data.sh | rocox -x counts -f >$ORACC_BUILDS/pctc/02xml/sl-corpus-counts.xml
