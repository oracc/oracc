#!/bin/sh
grep -v '@sense\|@entry\*' 00etc/edit-current.tab | \
    sed 's/@entry //g' | perl -p -e 's/\s*(\[.*?\])\s*/ $1 /' \
    >00etc/hist-glo-fix.tab
