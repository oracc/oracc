#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Util;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Bases;
use ORACC::CBD::Forms;

use Getopt::Long;

my $project = '';
my $lang = '';

my %args = pp_args();
my @cbd = setup_cbd(\%args);
my $cfgw = '';
my %need_forms = ();

for (my $i = 0; $i <= $#cbd; ++$i) {
    if ($cbd[$i] =~ /^\@entry\S*\s+(.*?)\s*$/) {
	$cfgw = $1;
    } elsif ($cbd[$i] =~ /^\@bases/) {
	my %h = bases_hash($cbd[$i]);
	my @b = grep(!/#/, keys %h);
	@b = map { tr/·°//d; $_ } @b;
	@need_forms{@b} = ();
    } elsif ($cbd[$i] =~ /^\@form\S*\s+(\S+)/) {
	delete $need_forms{$1};
    } elsif ($cbd[$i] =~ /^\@sense/) {
	if (scalar keys %need_forms) {
	    foreach my $f (keys %need_forms) {
		print "$cfgw\t$f /$f #~\n";
	    }
	    do {
		++$i;
	    } until ($cbd[$i] =~ /^\@end/);
	    %need_forms = ();
	}
    }
}

1;
