#!/usr/bin/perl
use warnings; use strict;

my $PQ = '';
my $last_PQ = '';
my $NM = '';
my $last_NM = '';
my %tlit = ();
my %tlat = ();
my %tlit_amp = ();
my @tlit = ();
my @tlat = ();
my $translation = 0;
my $last_amp = '';
my $pushed_amp = 0;

while (<>) {
    chomp;
    if (/^&([PQX]\S+)\s+/) {
	$PQ = $1;
	if ($tlit_amp{$_}) {
	    if ($tlit_amp{$_} ne $_) {
		warn "atftr.plx: $PQ: differing &-lines have same ID\n";
	    }
	}
	$last_amp = $_;
	$pushed_amp = $translation = 0;
    } elsif (/^\@translation/) {
	$translation = 1;
	push @{$tlat{$PQ}}, $last_amp;
	push @{$tlat{$PQ}}, $_;
    } elsif (/^&/) {
	warn "atftr.plx: bad &-line '$_'";
    } else {
	if ($translation) {
	    push @{$tlat{$PQ}}, $_;
	} else {
	    unless ($pushed_amp) {
		push @{$tlit{$PQ}}, $last_amp;
		$pushed_amp = 1;
	    }
	    push @{$tlit{$PQ}}, $_;
	}
    }
}

foreach my $id (sort keys %tlit) {
    print join("\n",@{$tlit{$id}}),"\n";
    if ($tlat{$id}) {
	my @tlat = @{$tlat{$id}};
	my $first = shift @tlat;
	until ($first =~ /^\&/) {
	    last if $#tlat < 0;
	    shift @tlat;
	}
	print "\n\n", join("\n",@tlat), "\n\n";
	delete $tlat{$id};
    }
}

foreach my $id (sort keys %tlat) {
    print "\n\n", join("\n\n",@{$tlat{$id}}), "\n\n";
}

1;
