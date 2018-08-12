#!/usr/bin/perl
use warnings; use strict;
my @t = ();
while (<>) {
    chomp;
    my($id,$freq,$refs) = split(/\t/,$_);
    print @t, "$id\t$freq\t".tissort($refs)."\n";
}

####

sub tissort {
    my @s = split(/\s/, $_[0]);
    join(' ',sort { &tiscmp; } @s);
}

sub tiscmp {
    my $a_pqx = $a; $a_pqx =~ s/^.*?://;
    my $b_pqx = $a; $b_pqx =~ s/^.*?://;
    $a cmp $b;
}

1;

    
