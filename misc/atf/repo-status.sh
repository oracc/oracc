#!/bin/sh
atf2xtf -c -lstatus/repo-status.log \
    -bstatus/repo-bad.txt -gstatus/repo-good.txt \
    sources/fullauto.atf
atfsplit2.plx -cat -list status/repo-bad.txt sources/fullauto.atf \
    >status/repo-bad.atf
sort status/repo-{good,bad}.txt >status/repo-list.txt
tools/repo-status.plx
