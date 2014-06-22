#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2P0::L2Super;
use Data::Dumper;

my %data = ORACC::L2P0::L2Super::init();

my %map_no_new = map_drop_act('new');

print Dumper \%map_no_new;

1;
