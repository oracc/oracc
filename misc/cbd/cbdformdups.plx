#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;

my %args = pp_args();
pp_file($$args{'cbd'});
my @cbd = pp_load($args);

if (pp_status()) {
    pp_diagnostics();
    exit 1;
}

my %f = ();
for (my $i = 0; $i <= $#cbd; ++$i) {
    next if $cbd[$i] =~ /^\000/;
    if ($cbd[$i] =~ /^\@form/) {
	my $fstart = $i;
	do {
	    $cbd[$i] =~ s/\s+/ /;
	    ++$f{$cbd[$i]};
	} while ($cbd[++$i] =~ /^\@form/);
	my $nforms = $i - $fstart;
	if (scalar keys %f < $nforms) {
	    warn "dropping dups at $i\n";
	    foreach my $k (sort keys %f) {
		$cbd[$fstart++] = $k;
	    }
	    while ($fstart < $i) {
		$cbd[$fstart++] = "\000";
	    }
	}
    }
}

pp_cbd(\%args,@cbd);

1;
