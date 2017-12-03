#!/bin/sh
project=$1
if [[ $project = "" ]]; then
    echo "rsync-project.sh: must give project. Stop."
    exit 1;
fi
cd $ORACC_BUILDS ; echo rsyncing $project in `pwd`
here=`hostname -f`
if [[ $here = "build-oracc.museum.upenn.edu" ]]; then
    there=oracc.ub.uni-muenchen.de
    port=22000
else
    there=build-oracc.museum.upenn.edu
    port=22
fi
src=$there:$ORACC_BUILDS/$project
if [ -d $project ]; then
    echo "rsync-project.sh: removing old version of $project"
    sudo rm -fr $project
fi
echo "rsync-project.sh: $src"
rsync -lr -e "ssh -p $port" $src .
sudo chown -R root:oracc $project
for a in bld pub tmp www xml ; do
    d=$ORACC_BUILDS/$a/$project
    s=$there:$d
    cd $ORACC_BUILDS/$a ;
    if [ -d $d ]; then
	echo "rsync-project.sh: removing old version of $d"
	sudo rm -fr $d
    fi
    echo "rsync-project.sh: $s"; rsync -lr -e "ssh -p $port" $s . ;
    sudo chown -R root:oracc $d
done
