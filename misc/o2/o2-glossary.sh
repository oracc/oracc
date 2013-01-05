#!/bin/sh
o2-cnf.sh
o2-glo.sh
[ -r 01bld/cancel ] && exit 1
[ -e 00lib/aliases.asa ] && cp 00lib/aliases.asa 02pub
o2-web.sh
o2-prm.sh
