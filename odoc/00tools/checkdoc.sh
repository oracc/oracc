#!/bin/sh
for a in `find ~/www/doc -name '*.html'` ; do 
    rnv -q ~/lib/schemas/xhtml/rnc/xhtml-strict.rnc $a ;
done
