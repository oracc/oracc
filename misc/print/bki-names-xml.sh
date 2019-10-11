#!/bin/sh
/bin/echo -n '<tab>' >00lib/names.xml
perl -p -e 's#^(\S+)\s+(.*)$#<t k="$1" v="$2"/>#' <00lib/names.tab >>00lib/names.xml
/bin/echo -n '</tab>' >>00lib/names.xml
