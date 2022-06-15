#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use Data::Dumper;

my %t = ();
my @p = eval("<$ENV{'ORACC_BUILDS'}/www/dcclt/lex-provides-*.xml>");

foreach my $p (@p) {
    my $type = type_of($p);
    foreach (`xmlnl <$p`) {
	if (/id=\"(o.*?)\"/) {
	    $t{$1} .= $type;
	}
    }
}

foreach (sort keys %t) {
    print "$_\t$t{$_}\n";
}

#################################################################
sub type_of {
    if ($_[0] =~ /base\./) {
	'b';
    } elsif ($_[0] =~ /phra\./) {
	'p';
    } else {
	'e';
    }
}

1;
