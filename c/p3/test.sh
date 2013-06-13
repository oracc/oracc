#!/bin/sh
qs=$1
echo $qs >t
len=`/bin/echo -n $qs | wc -c`
echo $qs length = $len
CONTENT_TYPE=application/x-www-form-urlencoded CONTENT_LENGTH=$len ./p3 <t
