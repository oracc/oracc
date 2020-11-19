#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; 
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";

# ensure every BASE occurs as a FORM -- use forms-uniq-form.plx to remove duplicates

my $lang = shift @ARGV || '';
$lang = " \%$lang" if $lang;

while (<>) {
    print;
    if (/^\@bases\s+(.*?)$/) {
	my $bb = $1;
	my @b = split(/; /, $bb);
	foreach my $b (@b) {
	    $b =~ s/\s\(.*$//;
	    $b =~ s/\*.*$//;
	    $b =~ s/^\s*//; $b =~ s/\s*$//;
	    my $b2 = $b;
	    $b =~ tr/·°//d;
	    print "\@form $b$lang /$b2 #~\n";
	}
    }
}

1;
