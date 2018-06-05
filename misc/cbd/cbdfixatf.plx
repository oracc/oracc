#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Fix;
use ORACC::CBD::Corpus;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;

$ORACC::CBD::nosetupargs = 1;

my %args = pp_args();
$args{'mode'} = 'corpus';

my @argv = @{$args{'argv'}};
foreach my $a (@argv) {
    my %fixed = pp_fix_file(\%args, $a);
    # pp_fix_file returns () if no fixes were made
    if (scalar keys %fixed) {
	my $outfile = "$a.new";
	pp_fix_new_atf($a, $outfile, \%fixed);
    }
}
pp_fix_close_log();
my $grand = pp_fix_grand_total();
warn "$0: $grand fixes made",
    ($grand ? "; see cbdfixatf.log for more information\n" : "\n");
1;
