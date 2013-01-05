#!/usr/bin/perl -C0
use warnings; use strict;
undef $/;
my @files = <*.atf>;
foreach my $f (@files) {
    open(F,$f); $_ = <F>; close(F);
    $_ =~ s/^.*?\&/\&/;
    open(F,">$f"); print F; close(F);
}

1;
