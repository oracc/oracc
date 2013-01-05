#!/bin/sh
B=${ORACC}/bin
C=`pwd`
(cd ${ORACC}/src/doc/ns/gdl/1.0 ; $B/xdfmanager.plx -rnc gdl.xdf)
(cd ${ORACC}/src/doc/ns/xtr/1.0 ; $B/xdfmanager.plx -rnc xtr.xdf)
(cd ${ORACC}/src/doc/ns/xtf/1.0 ; $B/xdfmanager.plx -rnc xtf.xdf)
(cd ${ORACC}/src/doc/ns/cdf/1.0 ; $B/xdfmanager.plx -rnc cdf.xdf)
(cd ${ORACC}/src/doc/ns/cdf/1.0 ; $C/mk_cdfrnc_c.plx >$C/cdf_rnc.c)
./mk_xmlnames.plx
