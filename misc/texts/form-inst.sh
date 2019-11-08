#!/bin/sh
ox -4 -I01bld/atfsources.lst | grep '	@' | cut -f2 | cut -d'%' -f2 | sed 's#//.*#]#' | sort -u
