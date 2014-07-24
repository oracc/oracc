#!/bin/sh
echo hello from doc-rebuild.sh
o2-web.sh
cd $ORACC/www
cp doc-redirect.html doc/index.html
