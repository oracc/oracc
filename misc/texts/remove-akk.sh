#!/bin/sh
grep '^&\|atf: lang' 00src/newtextsc.atf \
    | perl -n -e 'if(/^&(P[0-9]+)/){$x=$1}elsif(/lang akk/){print"$x\n"}' \
	   >>00lib/rejected.lst
sort -u -o 00lib/rejected.lst 00lib/rejected.lst
atfsplit.plx -cat -list 00lib/rejected.lst 00src/newtextsc.atf >>00any/akk-texts.atf
atfsplit.plx -cat -except -list 00lib/rejected.lst 00src/newtextsc.atf >sux-texts.atf
