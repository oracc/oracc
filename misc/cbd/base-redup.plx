#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";
use ORACC::CBD::Bases;

my $curr_cfgw = '';

bases_init();
while (<>) {
    if (/^[-+>]?\@(entry|bases)/) {
	if (/^[-+>]?\@entry\!?\s+(.*?)\s*$/) {
	    $curr_cfgw = $1;
	} else {
	    s/^\@bases\s+//;
	    my %b = bases_hash($_);
	    redup(grep(!/\#/, sort keys %b)) if scalar keys %b;
	}
    }
}
bases_term();

sub
redup {
    my @b = @_;
    # detect R-reduplication where writings are fully reduplicated
    my $RR = 0;
    my %solved = ();
    my @s = sort { length($b) <=> length($a) } @b;
    for (my $i = 0; $i < $#s; ++$i) {
	my $s = $s[$i];
	for (my $j = 0; $j <= $#s; ++$j) {
	    my $b = $s[$j];
	    $b =~ s/\{.*?\}//g;
	    my $bq = quotemeta($b);
	    my $ba = $b; $ba =~ tr/₀-₉//d;
	    my $baq = quotemeta($ba);
	    if ($s =~ m/^$bq-$bq-$bq-$bq$/ || $s =~ m/^$baq-$baq-$baq-$baq$/) {
		print "$curr_cfgw\t$s RRRR $b\n";
		++$solved{$i};
	    } elsif ($s =~ m/^$bq-$bq-$bq$/ || $s =~ m/^$baq-$baq-$baq$/) {
		print "$curr_cfgw\t$s RRR $b\n";
		++$solved{$i};
	    } elsif ($s =~ m/^$bq-$bq$/ || $s =~ m/^$baq-$baq$/) {
		print "$curr_cfgw\t$s RR $b\n";
		++$solved{$i};
	    }
	}
	unless ($solved{$i}) {
	    if ($s[$i] =~ /^(.*?)-\1$/) {
		print "$curr_cfgw\t$s[$i] *RR* $1\n";
		++$solved{$i};
	    } elsif ($s[$i] =~ /^.*?-(.*?)-\1$/) {
		print "$curr_cfgw\t$s[$i] RR* $1\n";
		++$solved{$i};
	    } elsif ($s[$i] =~ /^(.*?)-\1-$/) {
		print "$curr_cfgw\t$s[$i] *RR $1\n";
		++$solved{$i};
	    }
	}
    }
    for (my $i = 0; $i <= $#s; ++$i) {
	next if $solved{$i};
	# detect r-reduplication where writings are incompletely reduplicated
	my $b = $s[$i];
	$b =~ tr/₀-₉//d;
	$b =~ tr/-//d;
	$b =~ s/\{.*?\}//g;
	$b =~ s/([aeiu])\1/$1/g;
	my $c = $curr_cfgw; $c =~ s/\s+\[.*$//;
	next unless length($b) > length($c);
	# is the first base R and the second r ?
	if ($b =~ /^$c(.*)$/) {
	    my $bb = quotemeta($1);
	    # warn "found $c as subset of $b with remnant $bb\n" if $c eq 'hal';
	    if ($c eq $bb) {
		# warn "$c/$s[$i]: got $c eq $bb for RR'\n" if $c eq 'ša';
		print "$curr_cfgw\t$s[$i] RR' $c\n";
		++$solved{$i};
	    } elsif ($c =~ /^$bb/) {
		# warn "$c/$s[$i]: got $c contains $bb for Rr\n" if $c eq 'hal';
		print "$curr_cfgw\t$s[$i] Rr $c\n";
		++$solved{$i};
	    }	    
	}
	if (!$solved{$i}) {
	    # is the first base r and the second R ?
	    if ($b =~ /^(.*)$c$/) {
		my $bb = quotemeta($1);
		if ($c eq $bb) {
		    # warn "$c/$s[$i]: got $c eq $bb for RR'\n" if $c eq 'ša';
		    print "$curr_cfgw\t$s[$i] RR' $c\n";
		    ++$solved{$i};
		} elsif ($c =~ /^$bb/) {
		    # warn "$c/$s[$i]: got $c contains $bb for rR\n" if $c eq 'hal';
		    print "$curr_cfgw\t$s[$i] rR $c\n";
		    ++$solved{$i};
		}
	    }
	}
    }
}

1;
