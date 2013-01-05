#!/bin/sh
ood=${ORACC}/www/doc/ood
rm -fr  $ood ; mkdir -p $ood
xdfmanager.plx ; cp html/index.html $ood
for a in resources uris urns uri-patterns uri-text gdf; do
    (cd $a ; xdfmanager.plx ; mkdir -p $ood/$a; cp html/index.html $ood/$a)
done
chmod -R o+r $ood
