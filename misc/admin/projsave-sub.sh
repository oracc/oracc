#!/bin/sh
project=$1
backdir=$2
tarball=$3
user=`echo $project | cut -d/ -f1`
mkdir -p $backdir
chown $1:$1 $backdir
chmod g+r $backdir
chmod go-wx $backdir
tar zcf $tarball -T 01tmp/dist.lst && echo $1 project saveset written to $tarball
chown $user:$user $tarball
chmod o-rwx $tarball
