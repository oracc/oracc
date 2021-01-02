#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";

my $batch = '000';
my $size = 2500;

open(A,'01bld/lists/approved.lst') || die "$0: no '01bld/lists/approved.lst' to work on. Stop.\n";
batch_begin();
while (<A>) {
    if ($. % $size) {
	print;
    } else {
	batch_end();
	batch_begin();
	print;
    }
}
close(A);
batch_end();

sub batch_begin {
    my $b = "01bld/batch-$batch.lst";
    ++$batch;
    open(B,">$b") || die "$0: unable to open batch list $b\n";
    select B;
}

sub batch_end {
    close(B);
}

1;
