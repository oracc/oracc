#!/bin/sh
b=`basename $1 .t`
rm -fr atf/$b
rm -fr xml/$b
rm -fr tst/$b
mkdir -p atf/$b
mkdir -p xml/$b
mkdir -p tst/$b
echo "Processing '$b' tests"
while read l; do
    if [[ $l =~ ^[0-9] ]]; then
	read form
	read xml
#	echo testing $l against form $form and xml $xml
	t=`/bin/echo -n $l | cut -d. -f1`
	cat >atf/$b/$t.atf <<EOF
&X000001=Test
#project: test
$l
EOF
	cat >tst/$b/$t.grp <<EOF
$xml
EOF
	../ox/ox -f atf/$b/$t.atf >xml/$b/$t.xml 2>xml/$b/$t.log
	f=`xsltproc testform.xsl xml/$b/$t.xml`
	if [[ $f == $form ]]; then
	    echo "$l: $f = $form ok"
	else
	    echo "$1 $l failed form test $f != $form"
	fi
	xsltproc testxml.xsl xml/$b/$t.xml|xmllint --c14n - >tst/$b/$t.xml
	grep -qf tst/$b/$t.grp tst/$b/$t.xml || echo "$1 $l failed XML test"
    fi
done <$1

exit 0

