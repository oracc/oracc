#!/bin/sh
if [ "$ORACC" = "" ];
then
    echo uninstall.sh: must set ORACC environment variable first
    exit 1
fi
if [ "$ORACC_VAR" = "" ];
then
    echo uninstall.sh: must set ORACC_VAR environment variable first
    exit 1
fi
cd $ORACC ; rm -fr bin lib bld pub tmp www xml
rm -fr $ORACC_VAR
