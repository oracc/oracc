#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my %forms = ();

while (<>) {
    if (/^\@entry/) {
	warn "$.: missing \@end entry\n" if scalar keys %forms > 0;
	%forms = ();
    } elsif (s/^\@form\s*//) {
	chomp;
	s/\s+/ /;
	s/\s*$//;
	/^(\S+)/;
	$forms{$1} = $_;
	$_ = undef;
    } elsif (/^\@sense/ || /^\@end/) {
	foreach my $f (sort keys %forms) {
	    print "\@form $forms{$f}\n";
	}
	%forms = ();
    }
    print if $_;
}

1;
