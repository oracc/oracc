#!/bin/sh

# if there are <= $allthresh instances just inline them all
allthresh=30

function div_data {
    itot=$1
    s=$2
    
    if [[ $itot -gt $allthresh ]]; then
	allmode=0
	cat >>$o <<EOF
<h1 class="pcsl">$ict occurrence$s in PC corpus. <a href="/pctc/$oid.tis">See all instances</a></h1>
EOF
    else
	allmode=1
#	if [[ $ict -gt 1 ]]; then
#	    echo "<p><b>(All $itot instances shown below)</b></p>">>$o
#	fi
    fi
    
    declare -a v=(V IV III I)
    declare -a c=($V $IV $III $I)
    vlen=${#v[@]}
    for (( i=0; i<$vlen; ++i ));
    do
	if [[ "${c[$i]}" != "0" ]]; then
	    
	    if [[ "${c[$i]}" == "1" ]]; then
		s=""
	    else
		s="s"
	    fi
	    
	    if [[ "${v[$i]}" == "I" ]]; then
		when="ED"
	    else
		when="Uruk"
	    fi
	    
	    if [[ ${c[$i]} -ge 6 ]]; then

		if [[ $allmode == 1 ]]; then
		    cat >>$o <<EOF
<h2>${c[$i]} time$s in $when ${v[$i]}:</h2>
EOF
		else
		    cat >>$o <<EOF
<h2>${c[$i]} time$s in $when ${v[$i]}: <a href="/pctc/${oid}_$i.tis">see all $when ${v[i]} instances</a>.</h2>
EOF
		fi

	    elif [[ ${c[$i]} -gt 0 ]]; then
		if [[ ${c[$i]} == 1 ]]; then
		    cat >>$o <<EOF
<h2>1 time in $when ${v[$i]}:</h2>
EOF
		else
		    cat >>$o <<EOF
<h2>${c[$i]} times in $when ${v[$i]}:</h2>
EOF
		fi
	    fi
	
	    cat "${oid}_$i.div" >>$o
	fi
    done    
}

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
	I)
	    I=${a[$i+1]}
	    ;;
	*)
	    echo $0: unhandled case ${a[$i]}
    esac
    i=$((i+1))
done

if [[ "$V" == "" ]]; then
    V="0"
fi
if [[ "$IV" == "" ]]; then
    IV="0"
fi
if [[ "$III" == "" ]]; then
    III="0"
fi
if [[ "$I" == "" ]]; then
    I="0"
fi

itot=$(( $V + $IV + $III + $I))

o="$oid.html"
rm -fr $o
hh="${ORACC_BUILDS}/lib/data/pctc-xis-h.html"
ht="${ORACC_BUILDS}/lib/data/pctc-xis-t.html"

sed "s/@@TITLE@@/$oid Instance/" <"$hh" >$o

if [[ "$ict" != "0" ]]; then

    if [[ "$ict" == "1" ]]; then
	s=""
    else
	s="s"
    fi

    div_data $itot $s
else
    echo "<p>Not attested in PC corpus.</p>"
fi

cat "$ht" >>$o


#xsltproc --stringparam oid $oid \
#	 --stringparam ict $ict \
#	 --stringparam V $V \
#	 --stringparam IV $IV \
#	 --stringparam III $III \
#	 pctc-xis-html.xsl >$o
