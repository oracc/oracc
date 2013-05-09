#!/bin/sh
for a in `grep -lr 'al/oracc/' *` ; do
    perl -pi -e 's,/..r/local/oracc,\@\@ORACC\@\@,g' $a
done
