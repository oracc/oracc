#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::CBD::Util;
my $acd_rx = $ORACC::CBD::acd_rx;

my $multi = 0;
if ($ARGV[0] eq '-m') {
    $multi = 1;
    shift @ARGV;
}

my $file = $ARGV[0];

my $first_pos = '';
my $first_sense = '';

my @senses = ();

while (<>) {
    if (/^$acd_rx\@entry/) {
	$first_sense = '';
	print;
    } elsif (/^$acd_rx\@sense\+?\s+(\S+)\s+(.*?)\s*$/) {
	my $p = $1;
	my $s = $2;
	if ($first_sense) {
	    if ($first_sense =~ /$s/) {
		warn "$file:$.: suspicious second sense\n" unless $p ne $first_pos;
#	    } elsif ($s =~ /$first_sense/) {
#		warn "sux.glo:$.: suspicious first sense\n";
#		$first_sense = $s;
#		s/sense\+/sense/;
#		shift @senses;
#		push @senses, $_;
	    } else {
		warn "sux.glo:$.: multiple senses\n" if $multi;
		push @senses, $_;
	    }
	} else {
	    $first_sense = $s;
	    $first_pos = $p;
	    push @senses, $_;
	}
    } else {
	if ($#senses >= 0) {
	    print @senses;
	    @senses = ();
	}
	print;
    }
}

1;
