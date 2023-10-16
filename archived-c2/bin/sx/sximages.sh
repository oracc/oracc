#!/bin/sh
d=$1
if [[ "$d" == "" ]]; then
    echo "$0: must give sign-image directory on command line. Stop."
    exit 1
fi
if [ ! -d $d ]; then
    echo "$0: need valid directory with sign images on the command line. Stop."
    exit 1
fi
while read o n ; do
      if [ ! -r $d/$o.jpg ]; then
	  echo "$o	$n"
      fi
done
