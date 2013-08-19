#!/bin/sh
for a in `grep -lr 'al/oracc/' *` ; do
    perl -pi -e 's,/usr/local/oracc,\@\@ORACC\@\@,g' $a
done
