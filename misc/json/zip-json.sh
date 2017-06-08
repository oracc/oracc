#!/bin/sh
project=`oraccopt`
zip=`/bin/echo -n $project | tr / -` ;
zip="$ORACC_BUILDS/www/json/${zip}.zip" ;
(cd $ORACC_BUILDS/jsn ;
 rm -f $zip ; zip -q -r $zip $project
 chmod 0644 $zip ;
 )
/bin/echo -n $zip
