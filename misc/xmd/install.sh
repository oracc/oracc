#!/bin/sh
../install-one.sh
# Copy the XMD key data to lib/data
cp -p xmd/keydata.xml ${ORACC}/lib/data
