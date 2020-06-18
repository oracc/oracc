#!/bin/sh
grep -h '^add form ' $* | sed 's/add form //' | perl -p -e 's/^(.*?) => /$1\t/' | perl -p -e 's/(\[.*?\])/ $1 /' | sort -u
