#!/bin/sh
for a in `allprojlist.sh` ; do
    (ls -1 $ORACC/$a/00atf/* 2>/dev/null \
	| grep -v '\(bak\|log\|ods\|tmp\|~\)$') ;
done
