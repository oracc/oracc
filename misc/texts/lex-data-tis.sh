#!/bin/sh
ld=`oraccopt . cbd-word-phra`
if [ "$ld" == "" ]; then
    tis=''
elif [ "$ld" == "." ]; then
    tis=02pub/lex.tis
else
    tis=${ORACC_BUILDS}/pub/$ld/lex.tis
fi
if [ "$tis" != "" ]; then
    if [ -r $tis ]; then
	echo catting $tis
	cat $tis >>02pub/cbd/sux/sux.tis
    else
	echo no such file $tis
    fi
fi
