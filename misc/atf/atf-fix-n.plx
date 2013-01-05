#!/usr/bin/perl
use warnings; use strict;
use Getopt::Long;

my %forms = ();

while (<>) {
    if (/^\s*$/ || /^[&\@\$]/ || /^[:=]/ || /^\#/) {
	print;
    } else {
	my $line = $_;
	my $lem = <> || last;
	if ($lem =~ /^\#lem/) {
	    chomp $line; chomp $lem;
	    $line =~ s/^\S*\s+//;
	    $line =~ s/\s$//;
	    $lem =~ s/^\S*\s+//;
	    $lem =~ s/\s$//;
	    $line =~ s/<<.*?>>//g;
	    $line =~ s/--/-/;
#	    $line =~ tr/-:. a-zA-Z0-9\|\@&~%//cd;
	    $line =~ tr/<>[]\#\*\!\?//d;
	    $line =~ s/\s+/ /g;
	    $line =~ s/\(\d+://g;
	    $line =~ s/:\d+\)//g;
	    my @line = grep(defined&&length , split(/\s+/, $line));
	    my @lem = grep(defined&&length, split(/;\s+/, $lem));
	    if ($#line != $#lem) {
		warn("$.: $#line != $#lem\n");
	    } else {
		for (my $i = 0; $i <= $#line; ++$i) {
		    ++${$forms{$line[$i]}}{$lem[$i]};
		}
	    }
	}
    }
}

foreach my $f (sort keys %forms) {
    my $l = join('|', keys %{$forms{$f}});
    print "$f\t$l\n";
}

#######################################################

sub
bad {
    print STDERR 'atfglobal.plx: ', join('', @_), "\n";
}

sub
fail {
    bad(@_);
    die "atfglobal.plx: failed\n";
}

1;
