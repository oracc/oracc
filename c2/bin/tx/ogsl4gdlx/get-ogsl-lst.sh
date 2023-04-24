#!/bin/sh
../../sx/sx -r <../../sx/ogsl.asl | grep -i '[aeiun]' | sort -u >ogsl.lst
