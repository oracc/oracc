#!/bin/sh
cd /usr/local/oracc/www/runtimes
curl -O http://build.oracc.org/runtimes/$1
tar -C /usr/local/oracc -zxf $1
