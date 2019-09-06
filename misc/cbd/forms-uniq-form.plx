#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use lib "$ENV{'ORACC'}/lib";
use ORACC::CBD::Util;
my $acd_rx = $ORACC::CBD::acd_rx;

my %forms = ();

while (<>) {
    if (/^$acd_rx\@entry/) {
	warn "$.: missing \@end entry\n" if scalar keys %forms > 0;
	%forms = ();
    } elsif (s/^\@form\s*//) {
	chomp;
	s/\s+/ /;
	s/\s*$//;
	/^(\S+)/;
	$forms{$1} = $_ unless $forms{$1};
	$_ = undef;
    } elsif (/^$acd_rx\@sense/ || /^\@end/) {
	foreach my $f (sort keys %forms) {
	    print "\@form $forms{$f}\n";
	}
	%forms = ();
    }
    print if $_;
}

1;
