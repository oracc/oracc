#!/bin/sh
qs=$1
echo $qs >t
len=${#qs}
CONTENT_TYPE=application/x-www-form-urlencoded CONTENT_LENGTH=$len gdb ./p3
