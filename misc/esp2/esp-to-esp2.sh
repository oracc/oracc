#!/bin/sh
if [ ! -d 00any ]; then
    echo esp-to-esp2.sh: no 00any. Stop.
    exit 1
fi
e2=00any/e2
rm -fr $e2
mkdir -p $e2/00config $e2/00static
(cd 00web ; \
    find . -type f | grep -v '^./esp' | tar zcf - -T - | tar -C ../00any/e2/00static -zxf - )
(cd 00web/esp/site-content/00web ; \
    find . -type f  | tar zcf - -T - | tar -C ../../../../00any/e2/00static -zxf - )
(cd 00web/esp/site-content ; \
    cp -p *.xml ../../../00any/e2/00config)
(cd 00web/esp/site-content/pages ; \
    find . -type f  | tar zcf - -T - | tar -C ../../../../00any/e2 -zxf - )
if [ -f 00any/00web.tar.gz ]; then
    echo esp-to-esp2.sh: unable to save 00web to 00any/00web.tar.gz. 
    echo Please  rename 00any/00web.tar.gz and try again.
fi
tar zcf 00any/00web.tar.gz 00web || exit
rm -fr 00web
mv 00any/e2 00web
echo 00web has been converted to esp2.
