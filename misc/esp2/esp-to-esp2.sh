#!/bin/sh
if [ ! -d 00web/esp ]; then
    echo esp-to-esp2.sh: no 00web/esp to convert. Stop
    exit 1
fi
mkdir -p 00any
e2=00any/e2
rm -fr $e2
mkdir -p $e2/00config $e2/00res
(cd 00web ; \
    find . -type f | grep -v '^./esp' | tar zcf - -T - | tar -C ../00any/e2/00res -zxf - )
(cd 00web/esp/site-content/00web ; \
    find . -type f  | tar zcf - -T - | tar -C ../../../../00any/e2/00res -zxf - )
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
rm -fr 00res
mv 00web/00res 00res
mv 00res/*.* 00web
mv 00res/w3c 00web
echo 00web has been converted to esp2.
