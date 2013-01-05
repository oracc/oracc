#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Util;
my %h = ():
while (<>) {
    next if m/^\s*/
	|| m/^\@(project|name|lang)\s(.*?)$/
	|| m/^\{/
	|| m/\&\&/;
    chomp;
    my %sig = ORACC::L2GLO::Util::parse_sig($_);
    
}

1;
