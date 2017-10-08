#!/bin/sh
echo '### RUNNING epsd2-rebuild.sh ###' 
#epsd2-merge-forms.plx qpn 2>01tmp/qpn-merge.log
echo merging sux forms and glo
epsd2-merge-forms.plx sux 2>01tmp/sux-merge.log
epsd2-u3adm-sigs.plx
o2-superglo.sh
