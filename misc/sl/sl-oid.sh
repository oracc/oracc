#!/bin/sh
grep ^@sign 00lib/*.asl | perl -p -e 's/^.*?\s(\S+)$/sl\t$1\tsign/' | sort -u >01tmp/oid-sign.tab
grep ^@form 00lib/*.asl | perl -p -e 's/^\S+\s+\S+\s+(\S+).*$/sl\t$1\tsign/' | sort -u >01tmp/oid-form.tab
cat 01tmp/oid-{sign,form}.tab | oid.plx -a -d sl -p ogsl >02pub/sl-oid.tab
