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
	-v)
	    verbose=yes
	    shift # past argument
	    ;;
	*)
	    echo "usage:\n\n\tosync-tar-get.sh [-v] [-b | -m]\n"
	    echo "-v = verbose, more messages\n"
	    echo "-b to sync from build-oracc.museum.upenn.edu"
	    echo "-m to sync from oracc.ub.uni-muenchen.de\n"
	    exit 1
	    ;;	    
    esac
done

if [[ $hostarg = "" ]]; then
    echo "osync-tar-get.sh: must specify source as -b or -m. Stop."
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

(cd tar ; scp -P $port $there:/home/oracc/snc/*.tar.gz . )

ENDTIME=$(date +%s)

echo "osync-tar.sh: sync completed in $(( $ENDTIME - $STARTTIME )) seconds."

exit 0
