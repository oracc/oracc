#!/bin/sh
#
# Feed this script sigs via stdin and a project name on the command line to create
# a collection of glossarys in the directory sig-glo/
#
proj=$1
if [ "$proj" == "" ]; then
    echo "$0: must give project on command line. Stop."
    exit 1
fi
rm -fr sig-glo
mkdir -p sig-glo
cat >sig-glo/input.sig
# l can be empty here if we've previously restricted sigs to NN
l=`sig-langs.sh sig-glo/input.sig`
for a in $l ; do
    echo writing sig-glo/$a.glo
    grep '%'$a':' sig-glo/input.sig \
	| l2p2-sig-g2a.plx -filter -lang $a -proj $proj >sig-glo/$a.glo
done
echo writing sig-glo/qpn.glo
grep "\\][A-Z]N'" sig-glo/input.sig | l2p2-sig-g2a.plx -filter -lang qpn -proj $proj >sig-glo/qpn.glo
