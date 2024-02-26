#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my %f = ();
my %s = ();

while (<>) {
    chomp;
    my @f = split(/\t/,$_);
    if ($#f == 1) {
	++${$s{$f[0]}}{$f[1]};
    } else {
	my $fk = "$f[1]\t$f[2]";
	++${${$f{$f[0]}}{$fk}}{$f[3]};
    }
}

foreach my $s (sort keys %s) {
    print "\@sign\t$s\n";
    foreach my $v (sort keys %{$s{$s}}) {
	print "\@v\t$v\n";
    }
    if ($f{$s}) {
	foreach my $f (sort keys %{$f{$s}}) {
	    print "\@form $f\n";
	    if (${$f{$s}}{$f}) {
		foreach my $v (keys %{${$f{$s}}{$f}}) {
		    print "\@v\t$v\n";
		}
	    }
	    print "\@end form\n";
	}
    }
    print "\@end sign\n\n";
}

1;
