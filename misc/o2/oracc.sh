#!/bin/sh
# This script is just a shell wrapper so we can
# call the oracc program from su easily.
export -p ORACC ORACC_HOME
#echo oracc.sh $*
#echo -n oracc.sh pwd=
#pwd
exec "$ORACC/bin/oracc" $*
