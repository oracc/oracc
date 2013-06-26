#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDOUT, ':utf8';

my $chi_file = '';
my $reheads_file = '';
my $was_reheadped = 1;

use Getopt::Long;
Getopt::Long::GetOptions(
    'chi:s'=>\$chi_file,
    'rehead:s'=>\$reheads_file,
    'was:i'=>\$was_reheadped,
    );

die "chirehead.plx: no chi-file\n" unless $chi_file;
die "chirehead.plx: no reheads file\n" unless $reheads_file;

rehead($chi_file,$reheads_file);

sub
rehead {
    my($chi, $reheads) = @_;
    my %r = ();
    open(R, $reheads)  || die ("ORACC::CHI::Rehead: can't open reheads file list `$reheads'\n");
    while (<R>) {
	if (/(\S+)\tneeds-rehead\t(\S+)$/) {
	    $r{$1} = $2;
	} else {
	    warn "$reheads:$.: bad form in reheads file (say: CHI1<TAB>has-rehead<TAB>CHI2)\n";
	}
    }
    close(R);
    open(C, $chi) || die ("ORACC::CHI::Rehead: can't open chi-file `$chi' for reheadping\n");
    while (<C>) {
	if (/^\s*$/) {
	    print;
	    next;
	}
	unless (/^\@/) {
	    chomp;
	    my($t1,$t2,$t3) = (/^(\S+)\t(\S+)\t(.*?)$/);
	    my $has_r = undef;
	    if ($r{$t1}) {
		$t1 = $r{$t1};
	    }
	    if ($r{$t3}) {
		$t3 = $r{$t3};
	    }
	    if ($t3 !~ /^"/ && $t3 =~ /\./) {
		my @t3 = split(/(\.+-?)/, $t3);
		my @newt3 = ();
		foreach my $t (@t3) {
		    if ($r{$t}) {
			push @newt3, $r{$t};
		    } else {
			push @newt3, $t;
		    }
		}
		$t3 = join('', @newt3);
	    }
	    print "$t1\t$t2\t$t3\n";
	    print $has_r if $has_r;
	} else {
	    print;
	}
    }
    close(C);
}

1;
