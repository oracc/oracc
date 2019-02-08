#!/bin/sh
(cd signlist/02www ; 
 for a in `find l* -type f -name index.html | grep /x` ; \
     do (cd `dirname $a` ; xfrag -hH index.html innerContent >inner.html) ; \
     done \
     )
