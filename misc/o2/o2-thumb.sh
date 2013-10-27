#!/bin/sh
p=`oraccopt`
p=`echo $p | tr / _`
cp 00lib/$p-thumb.png 02www/images/$projbase-thumb.png
