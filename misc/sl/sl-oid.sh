#!/bin/sh
grep ^@sign 00lib/*.asl | perl -p -e 's/^.*?\s+(.*?)$/sl\t$1\tsign/' | oid.plx -a -d sl -p ogsl >02pub/sl-oid.tab
