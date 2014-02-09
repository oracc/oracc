#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $currsign = '';
my %lists = ();
my @signkeys = ();
my $signlist = '00lib/ogsl.asl';
my @signlist = `egrep '^\@\(sign\|list\|form\)' $signlist`;
my %signlists = ();
my %sortkeys = ();

foreach my $s (@signlist) {
    $s =~ s/\s+~\S+//;    
    if ($s =~ /^\@(?:sign|form)\s+(\S+)/) {
	$currsign = $1;
	push @{$signlists{$currsign}}, '#ogsl:';
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

my @sorted_sk = sort keys %signlists;
for (my $i = 0; $i <= $#sorted_sk; ++$i) {
    $sortkeys{"#ogsl:$sorted_sk[$i]"} = [ 'ogsl', $i ];
}

my @listnames =  map { "\L$_" } sort keys %lists;

foreach my $sk (sort @sorted_sk) {
    my %seen = ();
    print "$sk";
    my @skeys = @{$signlists{$sk}};
    foreach my $s (@skeys) {
	my $k;
	if ($s eq '#ogsl:') {
	    $k = $sortkeys{"#ogsl:$sk"};
	} else {
	    $k = $sortkeys{$s};
	}
	print "\t${$k}[0]=${$k}[1]"
	    unless $seen{$$k[0]}++;
    }
    foreach my $l (@listnames) {
	print "\t\L$l=\"100000\""
	    unless $seen{$l};
    }
    print "\n";
}

sub
listcmp {
    $a cmp $b;
}

1;
