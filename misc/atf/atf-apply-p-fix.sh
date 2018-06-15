#!/bin/sh
q="'"
perl -n -e 's#^.*?([PQX]\d+).*?=>\s+([PQX]\d+).*$#s/\\&$1/\\&$2/# && print' >atf.sed
#sed -f atf.sed $1
