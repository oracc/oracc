#!/bin/sh
(cd ${ORACC}/www ; \
    chmod -R o+r css doc js ; \
    find . -maxdepth 1 -type f -exec chmod o+r '{}' ';')
