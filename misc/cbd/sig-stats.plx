#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::L2GLO::Util;

my %seen = ();
my %stats = ();

while (<>) {
    next if /^\@fields/;
    # assume this is project.sig for now; could configure based on \@fields line
    my($sig,$rank,$count) = split(/\t/,$_);
    my %s = parse_sig($sig);
    next unless $s{'base'};

    my $cgp = "$s{'cf'}\[$s{'gw'}\]$s{'pos'}";

    if ($s{'base'} =~ s/^\%sux://) {
	${$stats{'bases'}}{"$cgp/".$s{'base'}} += $count;
    }

    unless ($seen{$cgp}++) {
	if ($s{'pos'} !~ /^.N$/) {
	    ++${$stats{'lemma'}}{'non-nn'};
	} else {
	    ++${$stats{'lemma'}}{'nn'};
	}
    }
}

# use Data::Dumper;
# print Dumper \%stats;

foreach my $b (sort { ${$stats{'bases'}}{$b} <=> ${$stats{'bases'}}{$a} } keys %{$stats{'bases'}}) {
    print "${$stats{'bases'}}{$b}\t$b\n";
}

1;
