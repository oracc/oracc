#!/usr/bin/perl
use warnings; use strict;
use Time::Local;
# you can do this with date --iso on Unix boxes, but
# not on Mac
my $time = time();
my @ltime = localtime($time);
my $gmtime = timegm(@ltime);
my $tz = $time - $gmtime;
$tz /= 60;
my $tz_h = $tz / 60;
my $tz_m = $tz % 60;
print "Content-type: text/xml\n\n";
printf("<date>%s-%02d-%02dT%02d:%02d:%02d-%02d%02d</date>",
       $ltime[5]+1900,$ltime[4]+1,
       $ltime[3],$ltime[2],$ltime[1],$ltime[0],
       $tz_h,$tz_m
    );
1;
