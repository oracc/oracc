#!/bin/sh
cd /Users/stinney/orc/www/runtimes
curl -O http://build.oracc.org/runtimes/$1
tar -C /Users/stinney/orc -Jxf $1
