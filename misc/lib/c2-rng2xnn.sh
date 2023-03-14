#!/bin/sh
rm -f rngnames.out
for a in *.rng; do
    xl $a | grep '\( ns=\|xmlns:\)' | \
	tr ' ' '\n' | tr -d '>"' | \
	grep -v '^<' >> rngnames.out
    xl $a | grep '<attribute name=\|<element name=' | \
	sed 's/ name=/=/g' | \
	tr -d '<>"/' | tr -s ' ' | sed 's/^ //' >>rngnames.out
done
grep -v xmlns= rngnames.out >$1
