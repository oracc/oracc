#!/bin/sh
while [[ $# -gt 0 ]]
do
    key="$1"
    case $key in
        -b)
            hostarg="$1"
	    host=build-oracc.museum.upenn.edu
	    port=22
            shift # past argument
	    ;;
        -m)
            hostarg="$1"
	    there=oracc.ub.uni-muenchen.de
	    port=22000
            shift # past argument
            ;;
        *)
            echo "usage:\n\n\tosync-all.sh [-b | -m]\n"
            echo "-b to sync from build-oracc.museum.upenn.edu"
            echo "-m to sync from oracc.ub.uni-muenchen.de\n"
            exit 1
            ;;
    esac
done

if [[ $hostarg = "" ]]; then
    echo "osync-all.sh: must specify host with -b or -m. Stop."
    exit 1;
fi

for a in `ssh -C -p $port $host ls -d /home/oracc/*/00lib | cut -d/ -f4`; do
    echo osync-project.sh -p $a $hostarg
done

echo "osync-all.sh: done."
exit 0
