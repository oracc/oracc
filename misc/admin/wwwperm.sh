#!/bin/sh
if [ "$1" = "" ]; then
    echo wwwperm.sh: must give project name as argument
    exit 1
fi
if [ $(whoami) = "root" ]
then
  user=`echo $1 | cut -d/ -f1` 
  chown -R $user:$user /usr/local/oracc/www/$1
  chmod -R o+r /usr/local/oracc/www/$1
else
    echo "wwwperm.sh: You can only run this script as root (preferably via cron)"
    exit 1
fi
