# 
# XML Site Architecture Project
# George MacKerron 2005
#
# Adapted for Oracc by Steve Tinney, 2010
#
# Adapted to esp2 by Steve Tinney, 2013

#ORACC=

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
    MSVJAR=$ORACC/lib/java/saxon.jar
    SAXONJAR=$ORACC/lib/java/msv.jar
    XSL=$ORACC/lib/scripts
    XMLSAP=$ORACC_HOME/$project/00web/esp
    ESP2=$ORACC/lib/esp2
    ESP2BIN=$ORACC/bin
    ESP2CP=$ORACC/lib/java
    XHTMLDTD=$ORACC/lib/schemas/xhtml/xhtml1-strict-esp2.dtd
fi

if [ ! -r $XMLSAP/00web/00config/structure.xml ]; then echo esp2.sh: no such ESP2 project $XMLSAP. Stop; exit 1; fi
if [ ! -r $SAXONJAR ]; then echo esp2.sh: no file saxon.jar. Stop.; exit 1; fi

SAXONPARAMS="oracc=$ORACC projesp=$XMLSAP scripts=$ESP2"

mkdir -p $XMLSAP/01tmp
mkdir -p $XMLSAP/02www/dev

HTTPROOT=$XMLSAP/02www/dev

#echo Removing existing development folder
if [ -d $HTTPROOT ]; then rm -rf $HTTPROOT; fi
mkdir $HTTPROOT

#echo Copying ESP default site content
for a in css img js ; do mkdir $HTTPROOT/$a ; cp $ESP2/$a/* $HTTPROOT/$a/ ; done

#echo Copying project site content
cd $XMLSAP/00web
cp -R * $HTTPROOT
rm -fr $HTTPROOT/00config

# Next line required or * misses hidden files like .htaccess that are direct children of static
cp .[a-zA-Z0-9]* $HTTPROOT

#echo Getting image info
#rm $XMLSAP/01tmp/images-info.txt
#cd $XMLSAP
#find 00web '(' -name '*.jpg' -or -name '*.png' -or -name '*.gif' ')' -print0 \
#    | xargs -0 java -classpath $ESP2CP ImageInfo \
#    >> $XMLSAP/01tmp/images-info.txt
#java -jar $SAXONJAR $XSL/esp2-images-info.xslt $XSL/esp2-images-info.xslt \
#    $SAXONPARAMS \
#    output-file=file:$XMLSAP/01tmp/images-info.xml project=$project

cd $XMLSAP && esp2-image-info.sh >01tmp/images-info.xml project=$project

#echo Getting page last-modified times
cd $XMLSAP/00web
find . -iname "*.xml" | $ESP2BIN/esp2-lmt.plx > $XMLSAP/01tmp/last-modified-times.xml

java -jar $SAXONJAR $XSL/esp2-stylesheets.xslt $XSL/esp2-stylesheets.xslt \
    $SAXONPARAMS \
    output-directory=file:$HTTPROOT project=$project

echo phase 1

java -jar $SAXONJAR $XMLSAP/00web/00config/structure.xml $XSL/esp2-phase-01.xslt \
    $SAXONPARAMS \
    output-file=file:$XMLSAP/01tmp/source-tree-10.xml project=$project

echo phase 2

java -jar $SAXONJAR $XMLSAP/01tmp/source-tree-10.xml \
    $SAXONPARAMS \
    $XSL/esp2-phase-02.xslt output-file=file:$XMLSAP/01tmp/source-tree-20.xml project=$project

echo phase 3

java -jar $SAXONJAR $XMLSAP/01tmp/source-tree-20.xml $XSL/esp2-phase-03.xslt \
    $SAXONPARAMS \
    output-file=file:$XMLSAP/01tmp/source-tree-30.xml project=$project

echo phase 4

java -jar $SAXONJAR $XMLSAP/01tmp/source-tree-30.xml $XSL/esp2-phase-04.xslt \
    $SAXONPARAMS \
    output-file=file:$XMLSAP/01tmp/source-tree-40.xml output-directory=file:$HTTPROOT project=$project

echo phase 5

java -jar $SAXONJAR $XMLSAP/01tmp/source-tree-40.xml $XSL/esp2-phase-05.xslt \
    $SAXONPARAMS \
    output-directory=file:$HTTPROOT project=$project

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

exit 0
