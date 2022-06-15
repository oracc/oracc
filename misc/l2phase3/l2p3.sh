#!/bin/sh

exec >01log/l2p3.log ; exec 2>&1

shopt -s nullglob
webdir=$1
shift 1
if [ "$webdir" == "" ]; then
    echo l2p3.sh: defaulting to webdir=01bld/www
    webdir=01bld/www
fi

cp -f 02xml/config.xml $webdir ; chmod o+r $webdir/config.xml

glostyle=`oraccopt . type`
if [ "$glostyle" == "superglo" ]; then
    xcf=l2p3-newstyle.xcf
else
    xcf=l2p3.xcf 
fi

inlinesenses=`oraccopt . cbd-inline-senses`
lexdata=`oraccopt . cbd-lex-data`

echo l2p3.sh: using configuration file $xcf

rm -fr $webdir/cbd ; mkdir -p $webdir/cbd
g2c=`find 01bld/* -maxdepth 1 -name '*.g2c'`
if [ "$g2c" != "" ]; then
    for g2c in `ls 01bld/*/*.g2c` ; do
	# -r fails if the glossary is empty because it points to *.cbd,
	# which doesn't exist
	if [ -r $g2c ]; then
	    ldir=`dirname $g2c`
	    l=`basename $ldir`
	    echo producing web version of $l
#	    echo "l2-glomanager -webdir=$webdir -conf $xcf -cbdlang $l $*"
	    l2-glomanager.plx -webdir=$webdir -conf $xcf -cbdlang $l $*
#	    echo g2c-sig-map
	    xsltproc $ORACC/lib/scripts/g2c-sig-map.xsl 01bld/$l/articles.xml >$webdir/cbd/$l/$l.map
#	    echo xff
	    xfftab=`oraccopt . cbd-forms-table`
	    if [ "$xfftab" = 'yes' ]; then
		l2p3-xff.sh $webdir $l
	    fi
	    if [ "$inlinesenses" == "yes" ]; then
		l2p3-inline-senses.sh $webdir $l
	    fi
	    if [ "$lexdata" == "yes" ]; then
		lex-xincludes.plx $webdir $l 2>lex-includes.log
	    fi	    
	fi
    done
fi

# Do this late so that all the necessary support files have been built
o2-lex.sh

# ## THIS IS NOW DONE IN l2p3-install-cbd.sh
# # if lexdata is being made the XHTML files are in 01tmp/lex/cbd/$lang/{pb,wp,eq}
# if [ -d 01tmp/lex/cbd/sux/pb ]; then
#     echo installing lex data cbd/sux/pb
#     mv 01tmp/lex/cbd/sux/pb $webdir/cbd/sux
# fi

# if [ -d 01tmp/lex/cbd/sux/wp ]; then
#     echo installing lex data cbd/sux/wp
#     mv 01tmp/lex/cbd/sux/wp $webdir/cbd/sux
# fi

# if [ -d 01tmp/lex/cbd/sux/eq ]; then
#     echo installing lex data cbd/sux/eq
#     mv 01tmp/lex/cbd/sux/eq $webdir/cbd/sux
# fi

# if [ -r 01bld/cat.geojson ]; then
#     mkdir -p $webdir/downloads
#     cp 01bld/cat.geojson $webdir/downloads
# fi

jumps=`oraccopt . outline-cbd-jumps`
if [ "$jumps" == "yes" ]; then
    ns-art-outlines.sh
fi
