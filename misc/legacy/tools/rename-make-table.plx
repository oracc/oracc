#!/usr/bin/perl
use warnings; use strict; use open 'utf8';

my @lines = `find atf -print0 -type f | xargs -0 grep '&[PQX][0-9][0-9][0-9][0-9][0-9][0-9]'`;

@lines = map { s/^.*?\&//; $_; } @lines;
open F,">filenames.tab" || die; select F;
foreach (@lines) {
    my($p,$n) = (/^(\S+)\s*=\s*(.*?)\s*$/);
    $n =~ s#[-/]\d+##g;
    $n =~ s/\s*\&.*$//;
    $n =~ tr/.,-//d;
    $n =~ s/^\s*(.*?)\s*$/$1/;
    $n =~ s/\s+/ /;
    $n =~ tr/ /_/;
    print "$p\t\L$n\n";
}
close(F);

warn "Files table written to filenames.tab; review before using\n";

1;
