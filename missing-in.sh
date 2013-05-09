#!/bin/sh
for a in `grep -lr 'al/ora' * | grep '\.\(sh\|plx\|xsl\)' | grep -v '~' | grep -v '\.in$'` ; do
    if [ ! -r $a.in ]; then
	echo $a
    fi
done
