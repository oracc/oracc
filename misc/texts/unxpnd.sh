#!/bin/sh
sed "s#^${ORACC_BUILDS}/bld/##" | sed 's/.xtf$//' | sed 's#/[PQX]...##' | sed 's#/[PQX]....../#:#'
