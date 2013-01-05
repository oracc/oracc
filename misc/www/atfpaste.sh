#!/bin/sh
perl -ne '($n,$l,$w)=(/^(.*?)\t(.*?)\t(.*?)$/);$n=~tr/ /_/;$l=~tr/ /_/;print"$n\:_$l\t$w\n"'