#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Getopt::Long;

my $errmsg = 0;
my $sort = 0;

GetOptions('s'=>\$sort,'e'=>\$errmsg);

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::SL::BaseC;

ORACC::SL::BaseC::init('g');
ORACC::SL::BaseC::pedantic();

if ($#ARGV >= 0) {
    my $t = shift @ARGV; Encode::_utf8_on($t);
    my $s = ORACC::SL::BaseC::tlit_sig('',$t);
    $s = sigsort($s) if $sort;
    print "$t => $s\n";
    if ($errmsg) {
	warn ORACC::SL::BaseC::messages(), "\n" if $s =~ /q0/;
    }
} else {
    while (<>) {
	chomp;
	my $t = $_;
	my $s = ORACC::SL::BaseC::tlit_sig('',$t);
	$s = sigsort($s) if $sort;
	print "$t => $s\n";
	if ($errmsg) {
	    warn ORACC::SL::BaseC::messages(), "\n" if $s =~ /q0/;
	}
    }
}

sub sigsort {
    my @s = split(/\./,$_[0]);
    join('.', sort @s);
}

1;
