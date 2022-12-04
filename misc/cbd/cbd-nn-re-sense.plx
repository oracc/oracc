#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

my $cf = '';
my $srcglo_arg = '';
my @srcglo = ();
my %x = ();

GetOptions(
    'srcglo:s'=>\$srcglo_arg,
    );

if ($srcglo_arg) {
    @srcglo = split(/,/,$srcglo_arg);
} else {
    push @srcglo, 'epsd2/names/00lib/qpn.glo', 'epsd2/emesal/00lib/sux-x-emesal.glo';
}

foreach my $s (@srcglo) {
    index_srcglo($s);
}

while (<>) {
    if (/^\@entry[!\*]*\s+(.*)\s*$/) {
	$cf = $1;
	$cf =~ s/\s*$//;
	print;
    } elsif (/^\@sense\s+(\S+)/) {
	my $epos = $1;
	my $xsns = $x{"$cf'$epos"};
	if ($xsns) {
	    print "$xsns\n";
	} else {
	    print;
	}
    } else {
	print;
    }
}


#######################################################################################

sub index_srcglo {
    my $g = shift;
    my $f = "$ENV{'ORACC_BUILDS'}/$g";
    if (-r $f) {
	my @s = `epsd2-pull-senses.plx $f`;
	chomp @s;
	foreach my $s (@s) {
	    next unless $s =~ /\] [A-Z]N\s/;
	    my($cgp,$sns) = split(/\t/, $s);
	    $sns =~ /\s(\S+)\s/;
	    my $epos = $1;
	    $x{"$cgp'$epos"} = $sns;
	}
    } else {
	warn "$0: can't index source glossary $f\n";
    }
}

1;
