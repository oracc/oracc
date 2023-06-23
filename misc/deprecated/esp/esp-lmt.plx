#!/usr/bin/perl -n
use warnings; use strict;
use POSIX qw(strftime);

BEGIN {
    print '<?xml version="1.0" encoding="utf-8"?>';
    print '<wm:last-modified-times xmlns:wm="http://oracc.org/ns/esp/1.0">'
}

chomp;
my @x = stat($_);
s#^./##;
printf "<wm:lmt file=\"$_\">%s</wm:lmt>", strftime '%d %b %G', localtime $x[9];

END {
    print '</wm:last-modified-times>';
}

1;
