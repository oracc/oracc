#!/bin/sh
sloid.sh pctc | pctc-count-data.sh \
    | tee 02pub/sl-corpus-counts.tab \
    | rocox -x counts -f \
	    >$ORACC_BUILDS/pctc/02xml/sl-corpus-counts.xml
