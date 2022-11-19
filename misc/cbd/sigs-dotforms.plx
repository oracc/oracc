#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use lib "$ENV{'ORACC_BUILDS'}/lib";

use ORACC::L2GLO::Util;

while (<>) {
    chomp;
    my %s = parse_sig($_);
    my $f = $s{'form'};
    $f =~ s/^.*?://;
    print "$s{'cf'} [$s{'gw'}] $s{'pos'}\t\%$s{'lang'}\t$f\n";
}

1;
