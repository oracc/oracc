#!/bin/sh
for f in `grep -rl 'url="http://oracc' 00web` ; do
    perl -pi -e 's,url="http://oracc.museum.upenn.edu,url=",g' $f
done
