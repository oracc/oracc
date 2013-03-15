# 
# XML Site Architecture Project
# George MacKerron 2005
#
# Adapted for Oracc by Steve Tinney, 2010
#

if [ "$1" = "" ]; then
    echo esp.sh: must give project name as argument
    exit 1
fi
project=$1
SAXONJAR=$ORACC/lib/java/saxon.jar
XMLSAP=$ORACC/$project/00web/esp
HTTPROOT=$XMLSAP/httproot_dev
XSL=$ORACC/lib/scripts

#echo Removing existing development folder
rm -rf $HTTPROOT
mkdir $HTTPROOT

#echo Copying ESP default site content
cd $ORACC/lib/esp
cp -R 00web/* $HTTPROOT
cp 00web/.* $HTTPROOT

#echo Copying project site content
cd $XMLSAP/site-content
cp -R 00web/* $HTTPROOT
# Next line required or * misses hidden files like .htaccess that are direct children of static
cp 00web/.* $HTTPROOT

#echo Getting image info
rm $XMLSAP/temporary-files/images-info.txt
cd $XMLSAP

#LIST="$(find site-content/static/images | grep '\(.*\.jpg$\|.*\.gif$\|.*\.png$\)')"
#if [ "$LIST" != "" ]; then
#    for F in "$LIST"
#    do 
#	java -classpath $ORACC/lib/java ImageInfo $F \
#	    >> $XMLSAP/temporary-files/images-info.txt
#    done
#fi

find site-content '(' -name '*.jpg' -or -name '*.png' -or -name '*.gif' ')' -print0 \
    | xargs -0 java -classpath $ORACC/lib/java ImageInfo \
    >> $XMLSAP/temporary-files/images-info.txt

#echo Getting page last-modified times
cd $XMLSAP/site-content/pages
find . -iname "*.xml" | esp-lmt.plx > $XMLSAP/temporary-files/last-modified-times.xml
##echo '<?xml version="1.0" encoding="utf-8"?>' > $XMLSAP/temporary-files/last-modified-times.xml
##echo '<wm:last-modified-times xmlns:wm="http://oracc.org/ns/esp/1.0">' >> $XMLSAP/temporary-files/last-modified-times.xml
##for FILE in `find . -iname "*.xml"`
##do 
##	ls -l -t $FILE | sed "s/.* \([0-9][0-9]*\) \([A-Z][a-z]*\) [0-9:]* \([0-9][0-9]*\) \.\/\(.*\)/\<wm:lmt file=\"\4\"\>\1 \2 \3\<\/wm:lmt\>/" >> $XMLSAP/temporary-files/last-modified-times.xml
##done
##echo '</wm:last-modified-times>' >> $XMLSAP/temporary-files/last-modified-times.xml

java -jar $SAXONJAR $XSL/esp-images-info.xslt $XSL/esp-images-info.xslt output-file=file:$XMLSAP/temporary-files/images-info.xml project=$project

java -jar $SAXONJAR $XSL/esp-stylesheets.xslt  $XSL/esp-stylesheets.xslt output-directory=file:$HTTPROOT project=$project

java -jar $SAXONJAR $XMLSAP/site-content/structure.xml $XSL/esp-phase-01.xslt output-file=file:$XMLSAP/temporary-files/source-tree-10.xml project=$project

java -jar $SAXONJAR $XMLSAP/temporary-files/source-tree-10.xml $XSL/esp-phase-02.xslt output-file=file:$XMLSAP/temporary-files/source-tree-20.xml project=$project

java -jar $SAXONJAR $XMLSAP/temporary-files/source-tree-20.xml $XSL/esp-phase-03.xslt output-file=file:$XMLSAP/temporary-files/source-tree-30.xml project=$project

java -jar $SAXONJAR $XMLSAP/temporary-files/source-tree-30.xml $XSL/esp-phase-04.xslt output-file=file:$XMLSAP/temporary-files/source-tree-40.xml output-directory=file:$HTTPROOT project=$project

java -jar $SAXONJAR $XMLSAP/temporary-files/source-tree-40.xml $XSL/esp-phase-05.xslt output-directory=file:$HTTPROOT project=$project

echo
echo "Validating pages as XHTML (only non-validating pages reported)"
echo
cd $XMLSAP
LIST="$(find $HTTPROOT -iname "*.html" | grep -v '/flashpages/')"
java -jar $ORACC/lib/java/msv.jar -standalone $ORACC/lib/schemas/xhtml/xhtml1-strict.dtd $LIST | grep -A 1 'Error' | esp-no-nmtok-err.plx

#echo "Setting X bits (chmod g+x) to enable SSIs, used for 'accessed' dates"
#echo
for FILE in $LIST                        
do
	chmod g+x $FILE
done

exit 0
