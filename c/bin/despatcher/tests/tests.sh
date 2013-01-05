#!/bin/sh
cd $ORACC/src/c/bin/despatcher/tests
SCRIPT_URL=/hbtin/P382239 ../oracc-despatcher >PQX-pager.out 2>PQX-pager.log
SCRIPT_URL=/hbtin/P382239/html ../oracc-despatcher >PQX-html.out 2>PQX-html.log
