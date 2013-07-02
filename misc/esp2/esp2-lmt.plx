#!/usr/bin/perl -n
use warnings; use strict;
use POSIX qw(strftime);

BEGIN {
    print '<?xml version="1.0" encoding="utf-8"?>';
    print '<esp:last-modified-times xmlns:esp="http://oracc.org/ns/esp/1.0">'
}

chomp;
my @x = stat($_);
s#^./##;
printf "<esp:lmt file=\"$_\">%s</esp:lmt>", strftime '%d %b %G', localtime $x[9];

END {
    print '</esp:last-modified-times>';
}

1;
