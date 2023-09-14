#!/bin/sh
declare -a a=(`cat data`)
alen=${#a[@]}
for (( i=0; i<$alen; ++i ));
do
    case ${a[$i]} in
	oid)
	    oid=${a[$i+1]}
	    ;;
	total)
	    ict=${a[$i+1]}
	    ;;
	V)
	    V=${a[$i+1]}
	    ;;
	IV)
	    IV=${a[$i+1]}
	    ;;
	III)
	    III=${a[$i+1]}
	    ;;
	*)
	    echo $0: unhandled case ${a[$i]}
    esac
    i=$((i+1))
done

echo found oid=$oid and ict=$ict
echo V=$V
echo IV=$IV
echo III=$III
