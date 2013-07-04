#!/usr/bin/perl
use warnings; use strict;

my($in,$out) = @ARGV;
my %collapsed = ();

open(IN,$in);
while (<IN>) {
    chomp;
    my $k = $_;
    $k =~ s/\..*$//;
    push @{$collapsed{$k}}, $_;
}
close(IN);

open(OUT,">$out");
foreach my $k (sort { &pqx_cmp } keys %collapsed) {
    print OUT join(',', @{$collapsed{$k}}), "\n";
}
close(OUT);

sub
pqx_cmp {
    my $akey = $a;
    my $bkey = $b;
    $akey =~ s/^.*?:(.*?)\..$/$1/;
    $bkey =~ s/^.*?:(.*?)\..$/$1/;
    return $akey cmp $bkey;
}

1;
