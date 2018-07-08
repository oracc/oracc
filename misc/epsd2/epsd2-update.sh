#!/bin/sh
echo '### RUNNING epsd2-update.sh ###' 
#echo merging sux forms and glo
#epsd2-merge-forms.plx sux 2>01tmp/sux-merge.log
o2-cnf.sh
cbdpp.plx 00src/sux.glo
l2p1.sh
