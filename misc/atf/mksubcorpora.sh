#!/bin/sh
rm -fr subcorpora
mkdir -p subcorpora/pc subcorpora/ed3a subcorpora/ed3b subcorpora/oa subcorpora/ur3
cat lists/uruk_{v,iv,iii}-have.lst lists/ed_i-have.lst >subcorpora/pc/pc.lst
cat lists/{ur_iii,lagash_ii}-have.lst >subcorpora/ur3/ur3.lst
atfsplit2.plx -list subcorpora/pc/pc.lst -cat sources/oraccifull.atf >subcorpora/pc/pc.atf
atfsplit2.plx -list lists/ed_iiia-have.lst -cat sources/oraccifull.atf >subcorpora/ed3a/ed3a.atf
atfsplit2.plx -list lists/ed_iiib-have.lst -cat sources/oraccifull.atf >subcorpora/ed3b/ed3b.atf
atfsplit2.plx -list subcorpora/ur3/ur3.lst -cat sources/oraccifull.atf >subcorpora/ur3/ur3.atf
