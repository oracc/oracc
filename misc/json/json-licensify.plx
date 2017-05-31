#!/usr/bin/perl
use lib "$ENV{'ORACC'}/lib";
use ORACC::JSON;
use warnings; use strict; use open 'utf8'; 
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

while (<>) {
    print;
    if (/\"project\"/) {
        my $defaultprops = ORACC::JSON::default_metadata($_,1);
        if ($defaultprops) {
            if (/,\s*$/) {
                chomp($defaultprops);
                $defaultprops .= ",\n";
            }
            print $defaultprops;
            while (<>) {
                print;
            }
	    last;
        }
    }
}

1;
