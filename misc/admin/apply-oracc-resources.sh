#!/bin/sh
for a in `find . -name 00res` ; do (cd `dirname $a` ; oracc resources); done
