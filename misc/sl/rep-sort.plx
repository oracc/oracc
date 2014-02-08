#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
my %signs = ();
while (<>) {
    chomp;
    my($signinfo,@values) = split(/\t/, $_);
    my($sign,$count) = (m#^(\S+)\s+(\d+)#);
    my $freq = shift @values;
    $signs{$sign} = [ $sign, $count, $freq , join("\t", @values) ];
}

foreach my $s (sort { ${$signs{$b}}[2] <=> ${$signs{$a}}[2] || ${$signs{$b}}[1] <=> ${$signs{$a}}[1] } keys %signs) {
    print "${$signs{$s}}[0]\t${$signs{$s}}[1]\t${$signs{$s}}[2]";
    my @vals = split(/\t/, ${$signs{$s}}[3]);
    foreach my $v (sort { &vcmp } @vals) {
	print "\t$v";
    }
    print "\n";
}

sub
vcmp {
    my($a_count) = ($a =~ m,(\d+)/,);
    my($b_count) = ($b =~ m,(\d+)/,);
    $b_count <=> $a_count;
}

1;
