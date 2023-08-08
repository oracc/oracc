#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use lib "$ENV{'ORACC'}/lib";
use ORACC::CBD::Util;
my $acd_rx = $ORACC::CBD::acd_rx;

use Getopt::Long;

my $key_is_full_form = 0;

GetOptions(
    full=>\$key_is_full_form,
    );

my @forms = ();
my %forms = ();

while (<>) {
    if (/^$acd_rx\@entry/) {
	warn "$.: missing \@end entry\n" if scalar keys %forms > 0;
	%forms = ();
	@forms = ();
    } elsif (s/^\@form\s*//) {
	chomp;
	s/\s+/ /;
	s/\s*$//;
	my $k = undef;
	if ($key_is_full_form) {
	    $k = $_;
	} else {
	    /^(\S+)/;
	    $k = $1;
	}
	push @forms, $_ unless $forms{$k}++;
	$_ = undef;
    } elsif (/^$acd_rx\@sense/ || /^\@end/) {
	foreach my $f (@forms) {
	    print "\@form $f\n";
	}
	%forms = ();
	@forms = ();
    }
    print if $_;
}

1;
