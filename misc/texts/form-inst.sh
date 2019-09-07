#!/bin/sh
ox -4 00atf/*.atf | grep '	@' | cut -f2 | cut -d'%' -f2 | sed 's#//.*#]#' | sort -u
