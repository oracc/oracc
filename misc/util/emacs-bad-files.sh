#!/bin/sh
if [ "$1" == "" ]; then echo 'emacs-bad-files.sh: must give name of log-file'; exit 1; fi
emacs `cut -d: -f1 $1 | sort -u  | grep ^00 | xargs echo`
