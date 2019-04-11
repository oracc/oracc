#!/usr/bin/perl
use warnings; use strict;

my @order = `cut -f1 00lib/designation-sort-order.lst`; chomp @order;
my @qname = `cat 01bld/lists/have-xtf.lst`; chomp @qname;
my @prox_qname = `cat 01bld/lists/proxy-xtf.lst`; chomp @prox_qname;

my %q = ();
foreach my $q (@qname, @prox_qname) {
    my($proj,$pqx) = ($q =~ /^(.*?):(.*?)$/);
    $q{$pqx} = $proj;
}

foreach my $o (@order) {
    if ($q{$o}) {
	print "$q{$o}\:$o\n";
    } else {
	warn "designation $o not in have-xtf.lst\n";
    }
}

1;
