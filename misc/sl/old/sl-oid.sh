#!/bin/sh
grep -E '^@(sign|nosign)' 00lib/*.asl | perl -p -e 's/^.*?\s(\S+)$/sl\t$1\tsign/' >01tmp/oid-sign.tab
grep ^@form 00lib/*.asl | perl -p -e 's/^\S+\s+\S+\s+(\S+).*$/sl\t$1\tsign/' >01tmp/oid-form.tab
sort -u 01tmp/oid-{sign,form}.tab | oid.plx -a -domain sl -p ogsl >02pub/sl-oid.tab
