#!/bin/sh
echo -n '<printindex>' >print/full/$1-ix.xml
for a in A B C D E F G NG H I J K L M N O P Q R S SH SS SA T TT U V W X Y Z ; \
    do [ -e website/epsd/$1-toc-$a.html ] && \
       xsltproc tools/index-html-print.xsl website/epsd/$1-toc-$a.html \
    >>print/full/$1-ix.xml ; \
done
echo -n '</printindex>' >>print/full/$1-ix.xml
