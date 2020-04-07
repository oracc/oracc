#!/bin/sh
user=`. oraccuser`
group=`. oraccgroup`
for a in `. oraccdirs.conf` ; do sudo chown -R $user:$group $a ; done
