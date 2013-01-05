#!/bin/sh
# This script is just a shell wrapper so we can
# call the oracc program from su easily.
ORACC=$1
shift
exec oracc $*
