#!/bin/sh
buildlex=`oraccopt . build-lex-data`
if [ "$buildlex" = "yes" ]; then
    proj=`oraccopt`
    if [ "$proj" = "dcclt" ]; then
	echo "o2-lex.sh: collecting lex data from  dcclt dcclt/nineveh dcclt/signlists"
	lex-sign-data.sh dcclt dcclt/nineveh dcclt/signlists
	lex-word-data.sh dcclt dcclt/nineveh dcclt/signlists
    else
	echo "o2-lex.sh: collecting lex data in $proj"
	lex-sign-data.sh $proj
	lex-word-data.sh $proj
    fi
    cp 01tmp/lex/*provides*.xml 02www ; chmod o+r 02www/*provides*.xml
fi
