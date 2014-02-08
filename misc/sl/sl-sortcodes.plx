#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $currsign = '';
my %lists = ();
my $signlist = '00lib/ogsl.asl';
my @signlist = `egrep '^\@\(sign\|list\|form\)' $signlist`;
my %signlists = ();
my %sortkeys = ();

foreach my $s (@signlist) {
    $s =~ s/\s+~\S+//;    
    if ($s =~ /^\@(?:sign|form)\s+(\S+)/) {
	$currsign = $1;
    } else {
	my($list) = ($s =~ /\s(\S+)/);
	my($prefix,$suffix) = ($list =~ /^([^\d]+)(\S+)\s*$/);
	push @{$signlists{$currsign}}, $list;
	push @{$lists{$prefix}}, $suffix;
    }
}

foreach my $l (keys %lists) {
    my @sorted = sort { &listcmp } @{$lists{$l}};
    for (my $i = 0; $i <= $#sorted; ++$i) {
	$sortkeys{"$l$sorted[$i]"} = ["\L$l" , $i];
    }
}

foreach my $s (sort keys %signlists) {
    my %seen = ();
    print "$s";
    my @skeys = @{$signlists{$s}};
    foreach my $s (@skeys) {
	my $k = $sortkeys{$s};
	print "\t${$k}[0]=${$k}[1]"
	    unless $seen{$$k[0]}++;
    }
    print "\n";
}

sub
listcmp {
    $a cmp $b;
}

1;
