#!/bin/sh
for a in 00lib/*.glo ; do
    cbdpp.plx -sig $a
done
