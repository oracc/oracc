#!/bin/sh
grep -v "\][A-Z]N'" $1 | perl -ne '@x{/%([-A-Za-z]+)(?:-[0-9]+)?:/}=();END{print join(" ", sort keys %x)}';
