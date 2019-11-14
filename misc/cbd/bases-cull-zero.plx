#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";
use ORACC::CBD::Util;
use ORACC::CBD::Bases;
$ORACC::CBD::Bases::ignore_empty_serializations = 1;
my $acd_rx = $ORACC::CBD::acd_rx;

my $bases_line = 0;
my %bases = ();
my %baserefs = ();
my $line = 0;

my @bases_used = `base-inst.sh`; chomp @bases_used; @bases_used = map { s/^.*?=//; $_; } @bases_used;
my %bases_used = (); @bases_used{@bases_used} = (); open(B,'>bases_used.dump'); print B Dumper \%bases_used; close(B);

my @out = ();
bases_init();
my $curr = '';
open(L,'>bases-cull-zero.log');
while (<>) {
    my $input = $_;
    if (/^$acd_rx\@entry\S*\s+(.*?)\s*$/) {
	$curr = $1;
	$curr =~ s/\s+\[/[/;
	$curr =~ s/\].*$/]/;
    } elsif (/^\@bases\s*/) {
	$bases_line = $#out+1;
	%bases = bases_hash($_);
	foreach my $b (keys %bases) {
	    next if $b =~ /#/;
	    if (exists $bases_used{"$curr/$b"}) {
		++$baserefs{$b} 
	    } else {
		print L "dropping unused base '$curr/$b'\n";
	    }
	}
    } elsif (/^\@end\s+entry/) {
	if ($bases_line > 0) {
	    # print "\@$bases_line: ", Dumper \%baserefs;
	    my @del = ();
	    foreach my $b (keys %bases) {
		next if $b =~ /#/;
		push @del, $b unless $baserefs{$b};
	    }
	    if ($#del >= 0) {
		foreach my $d (@del) {
		    delete $bases{$d};
		}
		my $n = bases_serialize(%bases);
		if ($n) {
		    $out[$bases_line] = "\@bases $n\n";
		} else {
		    1 while ($#out >= 0 && pop(@out) !~ /^\@entry/);
		    $input = '';
		}
	    }
	    %bases = ();
	    %baserefs = ();
	    $bases_line = -1;
	}
    }
    push(@out, $input) if $input;
}
bases_term();
close(L);
open(O,'>O'); print O Dumper \@out; close(O);
print @out;

1;
