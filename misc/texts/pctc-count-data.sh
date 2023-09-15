#!/bin/sh
echo "o	t	i"
while read o ; do
    f=01tmp/xis/$o/data
    if [ -r $f ]; then
	declare -a d=(`cat $f`)
	i=0
	if [ ${d[5]} -lt 6 ]; then
	    i=$(($i+${d[5]}))
	else
	    i=$(($i+5))
	fi
	if [ ${d[7]} -lt 6 ]; then
	    i=$(($i+${d[7]}))
	else
	    i=$(($i+5))
	fi
	if [ ${d[9]} -lt 6 ]; then
	    i=$(($i+${d[9]}))
	else
	    i=$(($i+5))
	fi
	if [ $i -eq 0 ]; then
	    i=100
	fi
	echo "$o	${d[3]}	$i"
    else
	echo "$o	0	0"
    fi
done
