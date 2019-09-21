#!/bin/sh
mkdir -p 00etc
find * -name history.edit -maxdepth 3 -exec cat '{}' ';' | sort -u >00etc/history.all
