#!/usr/bin/perl
use warnings; use strict;
use Data::Dumper;
use lib '@@ORACC@@/lib';
use ORACC::CHI::Printer;
use ORACC::CHI::Splitter;

open(D, ">test-printer.dump");
while (<>) {
    chomp;
    my @s = ORACC::CHI::Splitter::split_set($_);
    print D Dumper(@s);
    print ORACC::CHI::Printer::render(@s), "\n";
}
close(D);

1;
