#!/bin/sh
for a in `./missing-in.sh` ; do
    perl -p -e 's,/..r/local/oracc,\@\@ORACC\@\@,g' $a >$a.in
done
