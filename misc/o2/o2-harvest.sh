#!/bin/sh

report()
{
    n=`grep -c '@entry' 01bld/new/$l.new`
    if [ "$n" != "0" ]; then
	if [ "$n" == "1" ]; then
	    echo oracc harvest: $n entry for review in 01bld/new/$l.new
	else
	    echo oracc harvest: $n entries for review in 01bld/new/$l.new
	fi
    fi
}

movelang()
{
    (cd 01bld/new ; mv $l.srt $l.new) && report
}

slicelang()
{
    l=$1
    l2p2-sig-slicer.plx -lang $l -stdout -sigs 01bld/from-all-new.sig -exact >01bld/new/$l.sig
    cat 01bld/new/$l.sig | l2p2-sig-g2a.plx -filter -lang $l -proj `oraccopt` >01bld/new/$l.new
    [ -s 01bld/new/$l.new ] || rm -f 01bld/new/$l.new
}

sortlang()
{
    (cd 01bld/new ; l2-glomanager.plx -sort $l.new >$l.srt)
}

o2-atf.sh
o2-lst.sh
mkdir -p 01bld/new
l2p1-from-xtfs.plx -new
if [ -s 01bld/lists/proxy-lem.lst ]; then
    l2p1-from-xtfs.plx -proxy -new -textlist 01bld/lists/proxy-lem.lst
fi

cat 01bld/from-*-new.sig >01bld/from-all-new.sig

echo
echo ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
echo
for lang in `sig-langs.sh 01bld/from-*-new.sig` qpn ; do 
    slicelang $lang
    sortlang $lang && movelang
done
echo
echo ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
echo
echo oracc harvest: please review 01bld/new/*.new carefully. 
echo
echo After you are satisfied that all of the new entries are correct,
echo you can merge each language in turn using, e.g.:
echo
echo "	oracc merge akk-x-stdbab"
echo
echo See http://oracc.museum.upenn.edu/doc/manager/projunix for full 
echo instructions.
echo
echo ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
