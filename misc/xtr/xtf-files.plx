#!/usr/bin/perl
use warnings; use strict;

my @xtf_files = `cat 01bld/have-xtf.lst 01bld/proxy-atf | pqxpand xtf`;
foreach $x (@xtf_files) {
    $x =~ s/\.xtf/*.xtr/;
    my @xtr = eval "<01bld/$x>";
    print @xtr;
}

1;
