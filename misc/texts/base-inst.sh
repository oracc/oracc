#!/bin/sh
ox -4 -I01bld/atfsources.lst | grep '	@' | cut -f2 | cut -d'%' -f2 | grep '^sux' | cut -d'#' -f1 \
    | sed 's/+-.*//' | sed 's#//.*/#]/#' | sort -u
