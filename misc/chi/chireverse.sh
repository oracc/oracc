#!/bin/sh
perl -p -e 's/^(\S+)\t(\S+)\t(\S+)$/$3\t$2\t$1/' $1
