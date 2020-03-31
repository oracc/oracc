#!/bin/sh
if [ "$1" = "" ]; then
    echo projsave.sh: must give project name as argument
    exit 1
fi
if [ $(whoami) = "root" ]
then
  cd /home
  if [ -e $1 ]; then
    backdir=${VORACC}/bak/$1
    tarball=$backdir/$1-full-`date +%Y-%m-%d`.tar.xz
    find 00* -type f | grep -v '~$' >$1/01tmp/dist.lst
    ${ORACC}/bin/projsave-sub.sh $1 $backdir $tarball
  fi
else
    echo "You can only run this script as root (preferably via cron)"
    exit 1
fi
