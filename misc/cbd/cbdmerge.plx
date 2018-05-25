#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Hash;

my %args = pp_args();

my $base_hash = undef;
my $from_hash = undef;

$ORACC::CBD::nonormify = 1;

if ($args{'base'}) {
    my @cbd = setup_cbd(\%args,$args{'base'});
    
    if (($base_hash = pp_hash(\%args,@cbd))) {
	my $cbdname = ${$ORACC::CBD::data{'cbds'}}[0];
	open(B,'>base.dump');
	use Data::Dumper;
	print B Dumper \%{$ORACC::CBD::data{$cbdname}};
	close(B);
    } else {
	pp_diagnostics();
	exit 1;
    }
} else {
    die "cbdcompare.plx: must give base glossary with -base\n";
}

if (($from_hash = pp_hash(\%args))) {
    my $cbdname = ${$ORACC::CBD::data{'cbds'}}[1];
    open(D,'>from.dump');
    use Data::Dumper;
    print D Dumper \%{$ORACC::CBD::data{$cbdname}};
    close(D);
} else {
    pp_diagnostics();
}

my %b_acd = pp_hash_acd($base_hash);
my %f_acd = pp_hash_acd($from_hash);

my $merged = pp_acd_merge(\%b_acd, \%f_acd);
pp_acd_sort($merged); # sorts in-place
pp_serialize($base_hash,$merged);

open(M,'>merged.dump');
print M Dumper $merged;
close(M);

pp_diagnostics();

1;
