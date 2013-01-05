#!/bin/sh
for a in `${ORACC}/bin/projlist.sh` ; \
    do ${ORACC}/bin/projsave.sh $a ; \
done
