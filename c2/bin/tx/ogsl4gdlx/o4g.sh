#!/bin/sh
## This can soon be redone using sx ogsl.asl with a cli switch and no sed
./get-ogsl-lst.sh
./get-ogsl-pat.sh
paste ogsl.pat ogsl.lst >ogsl.tab
./ogsl-cc.plx ogsl.tab >ogsl-test.txt
## The preceding two lines can now be replaced by
### paste ogsl.lst ogsl.pat >ogsl-r.tab
### ../../xx/concord -sfwcl* <ogsl-r.tab
../gdlx -i -f ogsl-test.txt
