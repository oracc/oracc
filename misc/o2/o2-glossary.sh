#!/bin/sh
o2-cnf.sh
o2-glo.sh
[ -r 01bld/cancel ] && exit 1
[ -e 00lib/aliases.asa ] && cp 00lib/aliases.asa 02pub
o2-web.sh
l2p3.sh 01bld/www
for a in 02pub/cbd/* ; do 
    lang=`basename $a`
    if [ -r 01bld/$lang/letter_ids.tab ]; then
	cp 01bld/$lang/letter_ids.tab $a
	cp 01bld/$lang/L*.lst $a
    fi
done
o2-weblive.sh
o2-prm.sh
