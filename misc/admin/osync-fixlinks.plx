#!/usr/bin/perl
use warnings; use strict;

while (<>) {
    my($link,$source) = (/(\S+)\s+-?>\s+(\S+)/);
    $source =~ s#/home/oracc/#$ENV{'ORACC_BUILDS'}/#;
    my $fix ="rm -f $link ; ln -sf $source $link";
    print $fix, "\n";
#    system $fix;
}

1;
