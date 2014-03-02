#!/usr/bin/perl
use warnings; use strict;

my $keys = '';
my %keys = ();

use Getopt::Long;
GetOptions(
    'keys:s'=>\$keys,
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
    if ($keys) {
	/\&([PQX]\d+)/;
	$texts{$1} = $_;
    } else {
	/^.*=\s*(.*)\s*$/m;
	$texts{$1} = $_;
    }
}
 

if ($keys) {
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
