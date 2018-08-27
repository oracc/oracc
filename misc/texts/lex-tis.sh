#!/bin/sh
for a in base word tail ; do
    if [ -r 02pub/$a.tis ]; then
	cat 02pub/$a.tis >>02pub/cbd/sux/sux.tis
	(cd 02pub ; mv $a.tis $a.tis.done)
	echo lex-tis.sh added $a.tis to 02pub/cbd/sux/sux.tis and renamed it to $a.tis.done
    fi
done
