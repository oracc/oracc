#!/bin/sh
projects=$*
if [ "$projects" == "" ]; then
    echo 'lex-word-data.sh: must give project(s) on command line'
    exit 1
fi
lex-word-get-data.sh $*
lex-master.sh word
lex-word-base.sh
lex-word-qakk.sh
lex-word-phra.sh
