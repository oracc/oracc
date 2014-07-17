#!/bin/sh
echo hello from doc2-rebuild.sh
o2-web.sh
cd $ORACC/www
#sed 's,"./,"doc2/,g' <doc2/index.html >index.html
cp doc2-redirect.html doc2/index.html
#cat doc2/index.html
