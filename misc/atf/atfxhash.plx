#!/usr/bin/perl -C0
use warnings; use strict;
my %hashes = ();
my $col = '';
my $line = '';
my $loc = '';
my $surf = '';
my $p = '';

while (<>) {
    if (/^[\&\@]/ || m/^[^\@\$\#]\S*?\.\s/) {
	$loc = locator($_);
	print;
    } elsif (/^\#/) {
	if (/^\#\S+:\s/) {
	    print;
	} else {
	    chomp($_);
	    push @{$hashes{$p}}, [$loc, $_];
	}
    } else {
	print;
    }
}

#use Data::Dumper; print STDERR Dumper(\%hashes);
open C,">comments.csv";
foreach my $k (sort keys %hashes) {
    print C "\"$k\",\"";
    my @cmts = ();
    foreach my $k2 (@{$hashes{$k}}) {
	my($l,$c) = @$k2;
	$c =~ s/^\#\s+//;
	$c =~ s/\"/\"\"/g;
	$c =~ s/\s+$//;
	push @cmts, "#\@$l: $c";
    }
    print C join("\n",@cmts),"\"\n";
}
close(C);

#################################################

sub
locator {
    my %surface = (
	obverse=>'o',
	reverse=>'r',
	left=>'l.e.',
	right=>'r.e.',
	bottom=>'b.e.',
	top=>'t.e.',
	edge=>'e.',
	);
    if (/^\&(\S+)/) {
	$p = $1;
	$surf = $col = '';
    } elsif (/^\@(\S+)/) {
	if ($surface{$1}) {
	    $surf = $surface{$1};
	    $col = '';
	} else {
	    if (/^\@column\s+(\S+)/) {
		$col = $1;
	    }
	}
    } elsif (/^(\S+?)\.\s/) {
	$line = $1;
    }
    my $ret = "${surf}_${col}_$line";
    $ret =~ s/_+/_/g;
    $ret =~ s/^_//;
    $ret;
}

1;
