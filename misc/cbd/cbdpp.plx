#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use Getopt::Long;

my $cbd = shift @ARGV;
my $lng = $cbd; $lng =~ s/\.glo$//; $lng =~ s#.*?/([^/]+)$#$1#;

my @acd = ();
my @cbd = ();
my @ngm = ();

cbd_load();

dump_ngm() if $#ngm >= 0;
dump_acd() if $#acd >= 0; 
dump_cbd();

#######################################################################

sub cbd_load {
    open(C,$cbd) || die "cbdpp.plx: unable to open $cbd. Stop.\n";
    while (<C>) {
	chomp;
	if (/^\@ngm/) {
	    push @ngm, $_;
	    next;
	}
	if (/^\@bases/) {
	    my ($orig,$clean) = clean_bases($_);
	    if ($clean ne $orig) {
		push @acd, $clean;
	    }
	    push @cbd, $_;
	    next;
	}
	if (/^>.*?\@form/) {
	    push @acd, $_;
	    s/^.*?\@/\@/;
	    push @cbd, $_;
	    next;
	}
	if (/^[->+=]/) {
	    push @acd, $_;
	} else {
	    push @cbd, $_;
	}
    }
    close(C);
}

sub clean_bases {
    my $tmp = shift;
    my @b = split(/;\s+/, $tmp);
    my $rb = join('; ', @b);
    my @nb = ();
    foreach my $b (@b) {
	$b =~ s/^[-=+]//;
	$b =~ s/>\S+//;
    }
    ($rb, join('; ', @nb));
}

sub dump_acd {
    
}

sub dump_cbd {
    open(C, '>01bld/$lng.glo');
    foreach (@cbd) {
	print C $_, "\n";
    }
    close(C);
}

sub dump_ngm {
    open(N, '>01bld/$lng.ngm');
    foreach (@ngm) {
	print N $_, "\n";
    }
    close(N);
}

1;
