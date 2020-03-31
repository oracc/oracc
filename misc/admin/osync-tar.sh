#!/bin/sh
STARTTIME=$(date +%s)
verbose=no
while [[ $# -gt 0 ]]
do
    key="$1"
    case $key in
	-b)
	    hostarg="$1"
	    there=build-oracc.museum.upenn.edu
	    port=22
	    shift # past argument
	    ;;
	-m)
	    hostarg="$1"
	    there=oracc.ub.uni-muenchen.de
	    port=22000
	    shift # past argument
	    ;;
	-p)
	    project="$2"
	    shift # past argument
	    shift # past value
	    ;;
	-v)
	    verbose=yes
	    shift # past argument
	    ;;
	*)
	    echo "usage:\n\n\tosync-tar.sh [-v] -p PROJECT [-b | -m]\n"
	    echo "-p PROJECT=Oracc project name\n"
	    echo "-v = verbose, more messages\n"
	    echo "-b to sync from build-oracc.museum.upenn.edu"
	    echo "-m to sync from oracc.ub.uni-muenchen.de\n"
	    exit 1
	    ;;	    
    esac
done

if [[ $project = "" ]]; then
    echo "osync-tar.sh: must give project. Stop."
    exit 1;
fi

if [[ $hostarg = "" ]]; then
    echo "osync-tar.sh: must specify source as -b or -m. Stop."
    exit 1;
fi

here=`hostname`
if [[ $here = "build" ]]; then
    group=oracc
elif [[ $here = "oracc" ]]; then
    group=oracc
else
    group=staff
    here=localhost
fi

if [[ $verbose = "yes" ]]; then
    echo "osync-tar.sh: project=$project; there=$there; port=$port"
fi

oracc=/home/oracc
isproject=`ssh -p $port -C $there "[ -d $oracc/$project ] && echo yes || echo no"`

if [[ $isproject = "no" ]]; then
    echo "osync-tar.sh: project $project not in $there:$oracc"
    exit 1
fi

if [[ $oracc = $ORACC_BUILDS ]]; then
    fixlinks=no
else
    fixlinks=yes
fi

# Both build servers use /home/oracc for ORACC_BUILDS, so we
# use the actual variable for the server we are syncing TO, and
# we hardcode /home/oracc for $there because we know it
cd $ORACC_BUILDS ; echo osyncing $project in `pwd`
src=$there:$oracc/$project

if [ -d $project ]; then
    echo "osync-tar.sh: removing old version of $project"
    sudo rm -fr $project
    for a in bld pub tmp www xml ; do
	d=$ORACC_BUILDS/$a/$project
	s=$there:$oracc/$a/$project
	(cd $ORACC_BUILDS/$a ;
	    if [ -d $d ]; then
		echo "osync-tar.sh: removing old version of $d"
		sudo rm -fr $d
	    fi
	    )
    done
fi

echo "osync-tar.sh: $src"
scp -P $port $there:/home/oracc/snc/$project.tar.xz .
tar Jxpf $project.tar.xz
touch $project/.sync

if [[ $fixlinks = "yes" ]]; then
    cd $ORACC_BUILDS
    fixlinks=fixlinks-$$.sh
    echo '#!/bin/sh' >$fixlinks
    find $project/* -maxdepth 1 -type l -ls | osync-fixlinks.plx >>$fixlinks
    for a in bld xml ; do
	if [[ $a = "bld" ]]; then
	    find $a/$project/* -maxdepth 1 -type l -ls | osync-fixlinks.plx >>$fixlinks
	elif [[ $a = "xml" ]]; then
	    find $a/$project/oml -type l -ls | osync-fixlinks.plx >>$fixlinks
	fi
    done
    chmod +x $fixlinks
    sudo ./fixlinks-$$.sh
fi

ENDTIME=$(date +%s)

echo "osync-tar.sh: sync completed in $(( $ENDTIME - $STARTTIME )) seconds."

exit 0
