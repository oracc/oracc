#!/bin/sh
cat $* | grep -v "\][A-Z]N'" | perl -ne '@x{/%([-A-Za-z]+)(?:-[0-9]+)?:/}=();END{print join(" ", sort keys %x)}';
