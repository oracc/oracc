#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Getopt::Long;
my $sort = 0;
GetOptions('s'=>\$sort);

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::SL::BaseC;
ORACC::SL::BaseC::init();

if ($#ARGV >= 0) {
    my $t = shift @ARGV; Encode::_utf8_on($t);
    my $s = ORACC::SL::BaseC::tlit_sig('',$t);
    $s = sigsort($s) if $sort;
    print "$t => $s\n";
} else {
    while (<>) {
	chomp;
	my $t = $_;
	my $s = ORACC::SL::BaseC::tlit_sig('',$t);
	print "$t => $s\n";
    }
}

sub sigsort {
    my @s = split(/\./,$_[0]);
    join('.', sort @s);
}

1;
