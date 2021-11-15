#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;
use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::ATF::OX4;
use Data::Dumper;

open(OX4, "ox -4 @ARGV |");

ox4_init(\*OX4,1,1);
while (1) {
    my($ox4,$tok,$sigref) = ox4_next();
    last unless $ox4;
    print "$ox4: tok=$tok; sig=", Dumper $sigref;
}

close(OX4);

1;
