#!/bin/sh
# This script is just a shell wrapper so we can
# call the oracc program from su easily.
export -p ORACC ORACC_HOME
exec "$ORACC/bin/oracc" $*
