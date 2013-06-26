#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDOUT, ':utf8';

my $chi_file = '';
my $remaps_file = '';
my $was_remapped = 1;

use Getopt::Long;
Getopt::Long::GetOptions(
    'chi:s'=>\$chi_file,
    'remap:s'=>\$remaps_file,
    'was:i'=>\$was_remapped,
    );

die "chiremap.plx: no chi-file\n" unless $chi_file;
die "chiremap.plx: no remaps file\n" unless $remaps_file;

remap($chi_file,$remaps_file);

sub
remap {
    my($chi, $remaps) = @_;
    my %r = ();
    open(R, $remaps)  || die ("ORACC::CHI::Remap: can't open remaps file list `$remaps'\n");
    while (<R>) {
	if (/(\S+)\tneeds-remap\t(\S+)$/) {
	    $r{$1} = $2;
	} else {
	    warn "$remaps:$.: bad form in remaps file (say: CHI1<TAB>has-remap<TAB>CHI2)\n";
	}
    }
    close(R);
    open(C, $chi) || die ("ORACC::CHI::Remap: can't open chi-file `$chi' for remapping\n");
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
    foreach my $r (sort keys %r) {
	print "$r{$r}\twas-remapped-from\t$r\n";
    }
    close(C);
}

1;
