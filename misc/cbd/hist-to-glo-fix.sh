#!/bin/sh
cut -f3,5 00etc/history.edit | sed 's/@entry //g' | perl -p -e 's/\s*(\[.*?\])\s*/ $1 /' >00etc/hist-glo-fix.tab
