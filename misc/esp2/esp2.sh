# 
# XML Site Architecture Project
# George MacKerron 2005
#
# Adapted for Oracc by Steve Tinney, 2010
#
# Adapted to esp2 by Steve Tinney, 2013

#ORACC=

#function cp_00web_static {
#    echo Copying 00web/00static to web directory
#    cp -R * $HTTPROOT
#    # Next line required or * misses hidden files like .htaccess that are direct children of static
#    cp .[a-zA-Z0-9]* $HTTPROOT
#}

if [ "$1" = "" ]; then
    echo esp2.sh: must give project name as argument
    exit 1
fi
project=$1

if [ "$ORACC" = "" ]; then
    ORACC=~/oracc-sites
    ESP2=$ORACC/esp2
    MSVJAR=$ORACC/etc/msv/msv.jar
    SAXONJAR=$ORACC/etc/saxon/saxon*he.jar
    XSL=$ORACC/esp2
    XMLSAP=$ORACC/$project
    ESP2BIN=$ORACC/esp2
    ESP2CP=$ORACC/esp2/imageinfo-1_6:$ORACC/etc/saxon:$ORACC/etc/msv
    XHTMLDTD=$ESP2/dtd/xhtml1-strict-esp2.dtd
else
    MSVJAR=$ORACC/lib/java/msv.jar
    SAXONJAR=$ORACC/lib/java/saxon.jar
    XSL=$ORACC/lib/scripts
    XMLSAP=$ORACC_HOME/$project
    ESP2=$ORACC/lib/esp2
    ESP2BIN=$ORACC/bin
    ESP2CP=$ORACC/lib/java
    XHTMLDTD=$ORACC/lib/schemas/xhtml/xhtml1-strict-esp2.dtd
fi

if [ ! -r $XMLSAP/00web/00config/structure.xml ]; then echo esp2.sh: no such ESP2 project $XMLSAP. Stop; exit 1; fi
if [ ! -r $SAXONJAR ]; then echo esp2.sh: no file saxon.jar. Stop.; exit 1; fi

SAXONPARAMS="oracc=$ORACC projesp=$XMLSAP scripts=$ESP2 project=$project"

if [ ! -r $XMLSAP/01bld ]; then
    mkdir -p $XMLSAP/01bld
fi
if [ ! -r $XMLSAP/01tmp ]; then
    mkdir -p $XMLSAP/01tmp
fi
if [ ! -r $XMLSAP/02www ]; then
    mkdir -p $XMLSAP/02www
fi

HTTPROOT=$XMLSAP/01bld/espdev
HTTPLINK=$XMLSAP/02www/_test

#echo Removing existing development folder
if [ -d $HTTPROOT ]; then rm -rf $HTTPROOT; fi
mkdir $HTTPROOT
rm -f $HTTPLINK
ln -sf $HTTPROOT $HTTPLINK

echo Copying project static site content
# (cd $XMLSAP/00web/00static && cp_00web_static)
cp -R $XMLSAP/00res/* $HTTPROOT

#echo Copying ESP default site content
for a in css images js ; do mkdir -p $HTTPROOT/$a ; cp $ESP2/$a/* $HTTPROOT/$a/ ; done

cd $XMLSAP && esp2-image-info.sh >01tmp/images-info.xml project=$project

#echo Getting page last-modified times
cd $XMLSAP/00web
find . -iname "*.xml" | $ESP2BIN/esp2-lmt.plx > $XMLSAP/01tmp/last-modified-times.xml

java -jar $SAXONJAR $XSL/esp2-stylesheets.xslt $XSL/esp2-stylesheets.xslt \
    output-directory=file:$HTTPROOT $SAXONPARAMS

echo phase 1

java -jar $SAXONJAR $XMLSAP/00web/00config/structure.xml $XSL/esp2-phase-01.xslt \
    output-file=file:$XMLSAP/01tmp/source-tree-10.xml $SAXONPARAMS

echo phase 2

java -jar $SAXONJAR $XMLSAP/01tmp/source-tree-10.xml \
    $XSL/esp2-phase-02.xslt output-file=file:$XMLSAP/01tmp/source-tree-20.xml $SAXONPARAMS

echo phase 3

java -jar $SAXONJAR $XMLSAP/01tmp/source-tree-20.xml $XSL/esp2-phase-03.xslt \
    output-file=file:$XMLSAP/01tmp/source-tree-30.xml $SAXONPARAMS

xsltproc $XSL/esp2-fix-sort.xsl $XMLSAP/01tmp/source-tree-30.xml >$XMLSAP/01tmp/source-tree-31.xml

echo phase 4

java -jar $SAXONJAR $XMLSAP/01tmp/source-tree-31.xml $XSL/esp2-phase-04.xslt \
    output-file=file:$XMLSAP/01tmp/source-tree-40.xml output-directory=file:$HTTPROOT $SAXONPARAMS

echo phase 5

java -jar $SAXONJAR $XMLSAP/01tmp/source-tree-40.xml $XSL/esp2-phase-05.xslt \
    output-directory=file:$HTTPROOT $SAXONPARAMS

echo Validation

echo
echo "Validating pages as XHTML (only non-validating pages reported)"
echo
cd $XMLSAP
LIST="$(find $HTTPROOT -iname "*.html" | grep -v '/flashpages/')"
java -jar $MSVJAR -standalone $XHTMLDTD $LIST | grep -A 1 'Error' | $ESP2BIN/esp2-no-nmtok-err.plx

#echo "Setting X bits (chmod g+x) to enable SSIs, used for 'accessed' dates"
#echo
for FILE in $LIST                        
do
	chmod g+x $FILE
done

chmod -R o+r $HTTPROOT

exit 0
