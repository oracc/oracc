#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $p;
my %p = ();
my @text = ();

while (<>) {
    s/^\x{ef}\x{bb}\x{bf}//; # remove BOMs
    chomp;
    if (/^\&([PQX]\d+)/) {
	my $new_p = $1;
	if ($#text >= 0) {
	    $p{$p} = join("\n",@text);
	}
	$p = $new_p;
	@text = ($_);
    } else {
	push @text, $_;
    }
}

if ($#text >= 0) {
    $p{$p} = join("\n",@text);
}

foreach my $k (sort keys %p) {
    print $p{$k}, "\n\n";
}

1;
