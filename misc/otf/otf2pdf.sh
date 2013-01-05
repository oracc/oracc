#!/bin/sh
otf2odt.sh $1
odt2pdf.sh `basename $1 .cdf`.odt
