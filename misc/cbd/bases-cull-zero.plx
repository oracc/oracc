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

my @out = ();
bases_init();
while (<>) {
    if (/^$acd_rx\@entry/) {
    } elsif (/^\@bases\s*/) {
	%bases = bases_hash($_);
	$bases_line = $line;
    } elsif (/^\@form\s*/) {
	m#\s/(\S+)# && ++$baserefs{$1};
    } elsif (/^\@end\s+entry/) {
	# print "\@$bases_line: ", Dumper \%baserefs;
	my @del = ();
	foreach my $b (keys %bases) {
	    next if $b =~ /#/;
	    push @del, $b unless $baserefs{$b};
	}
	foreach my $d (@del) {
	    delete $bases{$d};
	}
	my $n = bases_serialize(%bases);
	$out[$bases_line] = "\@bases $n\n";
	%bases = ();
	%baserefs = ();
    }
    push @out, $_ if $_;
    ++$line;
}
bases_term();

print @out;

1;
