#!/bin/sh
cd /home
users=`ls -1d *`
echo users=$users
for a in $users; do
    if [ $a == "cdli" -o $a == "epsd2" ]; then
	echo skipping $a
    elif [ -r "$a/00lib/config.xml" ]; then
	echo backing up $a
	user-full-back.sh $a
    else
	echo ignoring $a
    fi
done

