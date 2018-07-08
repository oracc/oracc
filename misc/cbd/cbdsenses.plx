#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $multi = 0;
if ($ARGV[0] eq '-m') {
    $multi = 1;
    shift @ARGV;
}

my $first_sense = '';
my @senses = ();

while (<>) {
    if (/^\@entry/) {
	$first_sense = '';
	print;
    } elsif (/^\@sense\+?\s+\S+\s+(.*?)\s*$/) {
	my $s = $1;
	if ($first_sense) {
	    if ($first_sense =~ /$s/) {
		warn "sux.glo:$.: suspicious second sense\n";
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
