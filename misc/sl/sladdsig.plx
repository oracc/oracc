#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::SL::BaseC;

use Getopt::Long;

my $addtlit = 0;
my $sigfield = -1;
my $tlitfield = -1;

GetOptions(
    'a' => \$addtlit,
    's:i' => \$sigfield,
    't:i'  => \$tlitfield,
    );

ORACC::SL::BaseC::init();

while (<>) {
    chomp;
    my @f = split(/\t/,$_);
    $sigfield = $#f+1 unless $sigfield >= 0;
    unless (length $f[$sigfield]) {
	my $t = $f[$#f];
	my $s = ORACC::SL::BaseC::tlit_sig('',$t);
	## NEED TO DETECT ERRORS IN TLIT_SIG AND EXIT WITH ERROR STATUS
	if ($sigfield >= 0) {
	    $f[$sigfield] = $s;
	} else {
	    push @f, $s;
	}
	if ($tlitfield >= 0) {
	    my $st = join(' ',@ORACC::SL::BaseC::last_tlit);
	    $f[$tlitfield] = $st;
	} elsif ($addtlit) {
	    my $st = join(' ',@ORACC::SL::BaseC::last_tlit);
	    push @f, $st;
	}
	print join("\t",@f),"\n";
    } else {
	print $_, "\n";
    }
}

1;
