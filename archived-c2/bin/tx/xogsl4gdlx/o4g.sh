#!/bin/sh
./get-ogsl-lst.sh
./get-ogsl-pat.sh
paste ogsl.pat ogsl.lst >ogsl.tab
./ogsl-cc.plx ogsl.tab >ogsl-test.txt
../gdlx -i -f ogsl-test.txt
