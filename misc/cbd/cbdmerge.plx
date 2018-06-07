#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Hash;
use ORACC::CBD::Bases;

my %args = pp_args();

my $base_hash = undef;
my $from_hash = undef;

$ORACC::CBD::nonormify = 1;

#my $b1 = 'a (e)';
#my $b2 = 'e (esz10); duru (du)';
#my $b = bases_merge($b1,$b2,0);
#print "bases_string= ", bases_string($b), "\n";
#exit 0;

if ($args{'base'}) {
    my @cbd = setup_cbd(\%args,$args{'base'});
    
    if (($base_hash = pp_hash(\%args,@cbd))) {
	my $cbdname = ${$ORACC::CBD::data{'cbds'}}[0];
	if ($ORACC::CBD::PPWarn::trace) {
	    open(B,'>base.dump');
	    use Data::Dumper;
	    print B Dumper \%{$ORACC::CBD::data{$cbdname}};
	    close(B);
	}
    } else {
	pp_diagnostics();
	exit 1;
    }
} else {
    die "cbdmerge.plx: must give base glossary with -base\n";
}

if (($from_hash = pp_hash(\%args))) {
    my $cbdname = ${$ORACC::CBD::data{'cbds'}}[1];
    if ($ORACC::CBD::PPWarn::trace) {
	open(D,'>from.dump');
	use Data::Dumper;
	print D Dumper \%{$ORACC::CBD::data{$cbdname}};
	close(D);
    }
} else {
    pp_diagnostics();
    echo calling exit 1;
    exit 1;
}

unless (pp_status() || $args{'force'}) {
    my %b_acd = pp_hash_acd($base_hash);
    my %f_acd = pp_hash_acd($from_hash);
    
    my $merged = pp_acd_merge(\%b_acd, \%f_acd);
    pp_acd_sort($merged); # sorts in-place
    pp_serialize($base_hash,$merged);

    if ($ORACC::CBD::PPWarn::trace) {
	open(M,'>merged.dump');
	print M Dumper $merged;
	close(M);
    }
}

# pp_diagnostics();

1;
