#!/bin/sh
echo Setting docperms in $1
(cd $1 ; \
    [ -d css ] && chmod -R o+r css ; \
    [ -d doc ] && chmod -R o+r doc ; \
    [ -d js ] && chmod -R o+r js ; \
    find . -maxdepth 1 -type f -exec chmod o+r '{}' ';')
