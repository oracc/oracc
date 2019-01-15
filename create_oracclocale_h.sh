#!/bin/sh
ORACC_LOCALE=`locale -a | grep ^en | grep -i utf | head -1`
cat >c/include/oracclocale.h <<EOF
#define ORACC_LOCALE $ORACC_LOCALE
EOF

