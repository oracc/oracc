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

if [[ "$V" == "" ]]; then
    V="0"
fi
if [[ "$IV" == "" ]]; then
    IV="0"
fi
if [[ "$III" == "" ]]; then
    III="0"
fi

o="$oid.html"
rm -fr $o
hh="${ORACC_BUILDS}/lib/data/pctc-xis-h.html"
ht="${ORACC_BUILDS}/lib/data/pctc-xis-t.html"

sed "s/@@TITLE@@/$oid Instance/" <"$hh" >$o

if [[ "$ict" != "0" ]]; then

cat >>$o <<EOF
<h1 class="pcsl">$oid Instance Data: count=$ict <a href="/pctc/$oid.tis">See all instances</a></h1>
EOF

declare -a v=(V IV III)
declare -a c=($V $IV $III)
vlen=${#v[@]}
for (( i=0; i<$vlen; ++i ));
do
    if [[ "${c[$i]}" != "0" ]]; then
	cat >>$o <<EOF
<h2>Uruk ${v[$i]}: count=${c[$i]} <a href="/pctc/${oid}_$i.tis">see all Uruk ${v[i]} instances</a></h2>
EOF
cat "${oid}_$i.div" >>$o
    fi
done

else

    echo "<p>No instances attested.</p>"

fi

cat "$ht" >>$o


#xsltproc --stringparam oid $oid \
#	 --stringparam ict $ict \
#	 --stringparam V $V \
#	 --stringparam IV $IV \
#	 --stringparam III $III \
#	 pctc-xis-html.xsl >$o
