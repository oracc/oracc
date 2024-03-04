#!/bin/sh
cd
mkdir -p 00any/tex
cd 00any/tex 
xsltproc ~/00bin/gsl-TeX.xsl ~/02xml/ogsl.xml >gsl.tex
cdltex gsl.tex
