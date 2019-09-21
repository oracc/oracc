#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use Data::Dumper;
use ORACC::CBD::History;

if ($#ARGV >= 0) {
    my $g = "@ARGV";
    history_all_init();
    my $m = history_guess($g);
    print "$g => $m\n";
} else {
    history_trim();
}

1;
