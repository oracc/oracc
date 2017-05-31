#!/bin/sh
project=`oraccopt`
zip=`/bin/echo -n $project | tr / -` ;
zip="${zip}-json.zip" ;
(cd 01bld/json ;
 rm -f $zip ; zip -q -r $zip *.json corpusjson ;
 chmod 0644 $zip ;
 )
/bin/echo -n $zip
