#!/bin/sh
user=`cat .oraccuser`
group=`cat .oraccgroup`
for a in `. ./oraccdirs.conf` ; do sudo chown -R $user:$group $a ; done
