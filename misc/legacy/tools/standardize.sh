#!/bin/sh
atf2xtf convert/texts.atf | xsltproc ~/legacy/tools/xtf2atf.xsl - >convert/final.atf
