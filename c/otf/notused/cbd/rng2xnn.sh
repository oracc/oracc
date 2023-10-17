#!/bin/sh
rm -f rngnames.out
for a in $1/*.rng; do
    xl $a | grep '\( ns=\|xmlns:\)' | \
	tr ' ' '\n' | tr -d '>"' | \
	grep -v '^<' >> rngnames.out
    xl $a | grep '<attribute name=\|<element name=' | \
	sed 's/<\(attribute\|element\) name=/\1=/g' | \
	tr -d '>"/' | tr -s ' ' | sed 's/^ //' >>rngnames.out
done
grep -v xmlns= rngnames.out
