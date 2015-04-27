#!/bin/sh
pwd=`pwd`
for odt in `find odt -name '*.odt'` ; do
    dir=$pwd/`dirname $odt`
    xml=`basename $odt .odt`.xml
    tmp=/tmp/tmp$$ ; mkdir -p $tmp
    cd $tmp ; unzip -q $pwd/$odt
    echo converting $pwd/$odt to $dir/$xml
    echo '<?xml version="1.0"?>' >$dir/$xml
    echo '<pzip:archive xmlns:pzip="urn:cleverage:xmlns:post-processings:zip" xmlns:manifest="urn:oasis:names:tc:opendocument:xmlns:manifest:1.0" pzip:target="'$dir/$tmp'">' >>$dir/$xml
    echo '<pzip:entry pzip:target="mimetype" pzip:compression="none" pzip:content-type="text/plain" pzip:content="application/vnd.oasis.opendocument.text"/>' >>$dir/$xml
    for a in META-INF/manifest.xml content.xml styles.xml meta.xml settings.xml ; do
	echo '<pzip:entry pzip:target="'$a'">'>>$dir/$xml
	grep -v '^<?xml' $a >>$dir/$xml
	echo '</pzip:entry>' >>$dir/$xml
    done
    echo '</pzip:archive>' >>$dir/$xml
    cd ..
    rm -fr $tmp
done
