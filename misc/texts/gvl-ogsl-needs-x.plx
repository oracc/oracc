#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my %x = ();
my $fixq = '';

while (<>) {
    if (/ₓ/ && !/should be/) {
	if (/\[(\S+?) <= (\S+?)\]\s*$/) {
	    $fixq = $1;
	    warn "fixq = $fixq\n";
	}
	if (/\s(\S+?ₓ) unknown/) {
	    my $v = $1;
	    /Known for (\S+?):\s/ || /known values for (\S+)\s/;
	    my $q = $1;
	} elsif (/vq\] (\S+?ₓ)\((.*?)\):/) {
	    my $v = $1; my $q = $2;
	    needx($v,$q);
	} else {
	    warn $_;
	}
    }
}

foreach my $q (keys %x) {
    my @v = keys %{$x{$q}};
    print "$q\t@v\n";
}

############################################################################

sub needx {
    my($v,$q) = @_;
    return if $q =~ /X/;
    if ($fixq) {
	$q = $fixq;
	$fixq = '';
    }
    ++${$x{$q}}{$v};
}

1;
