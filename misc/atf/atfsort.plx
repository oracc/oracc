#!/usr/bin/perl
use warnings; use strict;

my $keys = '';
my %keys = ();
my $psort = 0;

use Getopt::Long;
GetOptions(
    'keys:s'=>\$keys,
    P=>\$psort,
    );

if ($keys) {
    open(K, $keys) || die;
    while (<K>) {
	chomp;
	$keys{$_} = $.;
    }
    close(K);
}

my %texts = ();
$/ = "\n\&";

my @file = (<>);

foreach (@file) {
    s/^[\n\t ]*\&?/&/;
    s/\&$//;
    if ($keys || $psort) {
	/\&([PQX]\d+)/;
	$texts{$1} = $_;
    } else {
	/^.*=\s*(.*)\s*$/m;
	$texts{$1} = $_;
    }
}
 

if ($psort) {
    foreach my $t (sort keys %texts) {
	print $texts{$t}, "\n";
    }    
} elsif ($keys) {
    foreach my $t (sort { &kcmp } keys %texts) {
	print $texts{$t};
    }    
} else {
    foreach my $t (sort { &tcmp } keys %texts) {
	print $texts{$t};
    }
}

sub
kcmp {
    $keys{$a} <=> $keys{$b};
}

sub
tcmp {
    $a cmp $b;
}

1;
