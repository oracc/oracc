#!/usr/bin/perl
foreach (<*.atf>) {
    chomp;
    my ($x,$y) = ($_,$_);
    $y =~ tr/ /_/;
    rename $x, $y;
}
1;
