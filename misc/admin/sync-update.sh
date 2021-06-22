#!/bin/sh
cd /home/oracc/www/runtimes
curl -O http://build.oracc.org/runtimes/$1
tar -C /home/oracc -Jxf $1
