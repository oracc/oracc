#!/bin/sh
find *.atf | perl -ne "'chomp;/^(.*?)$/;$x=$y=$1;$y=~tr/ /_/;rename($x,$y)\n'"
