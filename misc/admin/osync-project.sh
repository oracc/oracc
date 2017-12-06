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
	    echo "usage:\n\n\tosync-project.sh [-v] -p PROJECT [-b | -m]\n"
	    echo "PROJECT=Oracc project name\n"
	    echo "-v = verbose, more messages\n"
	    echo "-b to sync from build-oracc.museum.upenn.edu"
	    echo "-m to sync from oracc.ub.uni-muenchen.de\n"
	    exit 1
	    ;;	    
    esac
done

if [[ $project = "" ]]; then
    echo "osync-project.sh: must give project. Stop."
    exit 1;
fi

if [[ $hostarg = "" ]]; then
    echo "osync-project.sh: must specify source as -b or -m. Stop."
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
    echo "osync-project.sh: project=$project; there=$there; port=$port"
fi

oracc=/home/oracc
isproject=`ssh -p $port -C $there "[ -d $oracc/$project ] && echo yes || echo no"`

if [[ $isproject = "no" ]]; then
    echo "osync-project.sh: project $project not in $there:$oracc"
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
    echo "osync-project.sh: removing old version of $project"
    sudo rm -fr $project
fi
echo "osync-project.sh: $src"
rsync -lr -e "ssh -p $port" $src .

if [[ $fixlinks = "yes" ]]; then
    find $project/* -maxdepth 1 -type l -ls | osync-fixlinks.plx
fi

sudo chown -R root:$group $project

for a in bld pub tmp www xml ; do
    d=$ORACC_BUILDS/$a/$project
    s=$there:$oracc/$a/$project
    cd $ORACC_BUILDS/$a ;
    if [ -d $d ]; then
	echo "osync-project.sh: removing old version of $d"
	sudo rm -fr $d
    fi
    echo "osync-project.sh: $s"; 
    rsync -lr -e "ssh -p $port" $s .

    if [[ $fixlinks = "yes" ]]; then
	if [[ $a = "xml" ]]; then
	    find $project/oml -type l -ls | osync-fixlinks.plx
	elif [[ $a = "bld" ]]; then
	    find $project/* -maxdepth 1 -type l -ls | osync-fixlinks.plx
	fi
    fi

    sudo chown -R root:$group $d

done

sudo serve-index.sh $project $there

ENDTIME=$(date +%s)

echo "osync-project.sh: sync completed in $(( $ENDTIME - $STARTTIME )) seconds."

exit 0
