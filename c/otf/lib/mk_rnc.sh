#!/bin/sh
B=${exec_prefix}/bin
C=`pwd`
(cd ../../../doc/ns/gdl/1.0 ; $B/xdfmanager.plx -rnc gdl.xdf)
(cd ../../../doc/ns/xtr/1.0 ; $B/xdfmanager.plx -rnc xtr.xdf)
(cd ../../../doc/ns/xtf/1.0 ; $B/xdfmanager.plx -rnc xtf.xdf)
(cd ../../../doc/ns/cdf/1.0 ; $B/xdfmanager.plx -rnc cdf.xdf)
(cd ../../../doc/ns/cdf/1.0 ; $C/mk_cdfrnc_c.plx >$C/cdf_rnc.c)
./mk_xmlnames.plx
