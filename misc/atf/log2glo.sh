#!/bin/sh
mk-atf-reptab.plx log | sort -u >new.forms
grep '\][A-Z]N ' new.forms >qpn.forms
grep '\][A-Z]N ' new.forms >sux.forms
dotforms-sigs.plx qpn.forms | l2p2-sig-g2a.plx -filter -lang sux -proj eisl >emegir.glo
dotforms-sigs.plx sux.forms | l2p2-sig-g2a.plx -filter -lang sux -proj eisl >emesal.glo
echo Checking emegir.glo
cbdpp.plx -c emegir.glo
echo Checking emesal.glo
cbdpp.plx -c emesal.glo
