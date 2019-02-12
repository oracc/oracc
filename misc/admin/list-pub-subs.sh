#!/bin/sh
for a in `list-subprojects.sh $1`; do
    yn=`oraccopt $a public` ;
    if [ "$yn" = "yes" ]; then echo $a ; fi;
done
