#!/bin/sh
pwd
for a in `. ./oraccdirs.conf` ; do mkdir -p $a ; done
