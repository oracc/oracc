#!/bin/sh
rm -fr convert
mkdir -p convert/odt
OdfConverter /I orig /R /O convert/odt /LEVEL 4 /BATCH-DOCX /NOPACKAGING >/dev/null
