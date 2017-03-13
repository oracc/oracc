#!/bin/sh
log=$1
env=$2
. $env
echo '### RUNNING epsd2-init.sh ###' >>$log
#epsd2-merge-forms.plx qpn 2>01tmp/qpn-merge.log
epsd2-merge-forms.plx sux 2>01tmp/sux-merge.log
