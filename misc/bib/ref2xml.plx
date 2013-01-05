#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
$/ = "\n\n";
print "<list>\n";
while (<>) {
    print "<item>\n";
    foreach my $f (split(/\n/,$_)) {
	next if /^\s*$/;
	my($tag,$con) = ($f =~ /^%(.)\s+~*(.*)/);
	next unless length($tag) && length($con);
	$con =~ s/\s*$//;
	if (($tag eq 'A' || $tag eq 'E') && !/, /) {
	    $con =~ s/^(.*?)\s+(\S+)$/$2, $1/;
	}
	$con =~ tr/_~/  /;
	print "<$tag>$con</$tag>\n";
    }
    print "</item>\n";
}
print "</list>\n";

1;
