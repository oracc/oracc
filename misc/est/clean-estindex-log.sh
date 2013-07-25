#!/bin/sh
perl -i -n -e '/^estcmd: INFO/ || print' 01tmp/estindex.log
