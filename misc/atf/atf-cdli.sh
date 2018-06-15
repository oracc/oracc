#!/bin/sh
se \#cdli \!cat OB_Contracts | sed 's/^cdli://' >atf-cdli.res
grep -f atf-cdli.res $ORACC_BUILDS/pub/cdli/Pnames.tab >atf-cdli.lst
