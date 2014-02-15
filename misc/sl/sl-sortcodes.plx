#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $currform = '';
my $currsign = '';
my %lists = ();
my @signkeys = ();
my $signlist = '00lib/ogsl.asl';
my @signlist = `egrep '^\@\(sign\|list\)' $signlist`; # removed 'form'
my %signlists = ();
my %formlists = ();
my %sortkeys = ();

foreach my $s (@signlist) {
    if ($s =~ /^\@sign\s+(\S+)/) {
	$currsign = $1;
	if ($signlists{$currsign}) {
	    warn "sl-sortcodes.plx: duplicate SIGN $currsign\n";
	    next;
	} else {
	    push @{$signlists{$currsign}}, '#ogsl:';
	}
#    } elsif ($s =~ /^\@form\s+~\S+\s+(\S+)/) {
#	$currform = $1;
#	if ($formlists{$currform}) {
#	    warn "sl-sortcodes.plx: duplicate FORM $currsign\n"
#		unless $s =~ /form/;
#	    next;
#	} else {
#	    push @{$formlists{$currform}}, '#ogsl:';
#	}
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

my @sorted_sk = sort { &kcmp } keys %signlists;
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
kcmp {
    my($akey,$bkey) = ($a,$b);
    $akey =~ tr/\|//d;
    $bkey =~ tr/\|//d;
    my ($anum,$alet) = ($akey =~ /^(\d*)\(?(\S+)/);
    my ($bnum,$blet) = ($bkey =~ /^(\d*)\(?(\S+)/);
    my $res = 0;
    if ($anum && $bnum) {
	$res = $anum <=> $bnum;
	return $res unless $res == 0;
    } elsif ($anum) {
	return 1;
    } elsif ($bnum) {
	return -1;
    }
    return $alet cmp $blet;
}

sub
listcmp {
    $a cmp $b;
}

1;
