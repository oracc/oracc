#!/usr/bin/perl
use warnings; use strict;
use File::Find;
my $reffile = shift @ARGV;
my $refdate = ($reffile ? (stat($reffile))[9] : 0);
#warn "$refdate\n";
my @dirs = grep !/00any/, <00*>;
find(\&wanted, @dirs);
sub
wanted {
    if (-f $_) {
	if (/~$/
	    || $File::Find::name =~ m#00web/esp/(?:http|back|temp)#) {
	} else {
	    if ($refdate) {
		my $fdate = (stat($_))[9];
		print "$File::Find::name\n"
		    if $fdate > $refdate;
	    } else {
		print "$File::Find::name\n";
	    }
	}
    }
}
1;
