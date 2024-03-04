#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::SL::BaseC;
ORACC::SL::BaseC::pedantic();
ORACC::SL::BaseC::init();
if ($#ARGV >= 0) {
    foreach (@ARGV) {
	do_one("args");
    }
} else {
    while (<>) {
	chomp;
	do_one("<stdin>:$.");
    }
}
ORACC::SL::BaseC::term();

sub do_one {
    my $context = shift;
    my $res = ORACC::SL::BaseC::check($context,$_);
    print "$res\n";
    print join("\n", ORACC::SL::BaseC::messages(), '');
}

1;
