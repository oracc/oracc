#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";

my $batch = '000';
my $printed = 0;
my $size = 2500;
my $start = 0;

open(A,'01bld/lists/approved.lst') || die "$0: no '01bld/lists/approved.lst' to work on. Stop.\n";
my @a = (<A>);
close(A);

while ($printed <= $#a) {
    batch_begin();
    my $end = $start + $size - 1;
    $end = $#a if $end > $#a;
    print @a[$start .. $end];
    batch_end();
    $printed += $size;
    $start += $size;
}

batch_end();

sub batch_begin {
    my $b = "01bld/atf-batch-$batch.lst";
    ++$batch;
    open(B,"|pqxpand atf >$b") || die "$0: unable to open batch list $b\n";
    select B;
}

sub batch_end {
    close(B);
}

1;
