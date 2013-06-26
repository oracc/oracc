package ORACC::CHI::Remap;
use warnings; use strict;

sub
remap {
    my($chi, $remaps) = @_;
    my %r = ();
    open(R, $remaps)  || die ("ORACC::CHI::Remap: can't open remaps file list `$remaps'\n");
    while (<R>) {
	if (/(\S+)\thas-remap\t(\S+)$/) {
	    $r{$1} = $2;
	} else {
	    warn "$remaps:$.: bad form in remaps file (say: CHI1<TAB>has-remap<TAB>CHI2)\n";
	}
    }
    close(R);
    open(C, $chi) || die ("ORACC::CHI::Remap: can't open chi-file `$chi' for remapping\n");
    while (<C>) {
	unless (/^\@/) {
	    chomp;
	    my($t1,$t2,$t3) = (/^(\S+)\t(\S+)\t(.*?)$/);
	    my $has_r = undef;
	    if ($r{$t1)) {
		$t1 = $r{$t1};
		if ($t2 eq '_terminal') {
		    $has_r = "$r{$t1}\thas-remap\t$t1\n";
		}
	    } elsif ($r{$t3}) {
		$t3 = $r{$t3};
	    } elsif ($t3 =~ /\./) {
		my @t3 = split(/(\.+-?)/, $t3);
		my @newt3 = ();
		foreach my $t (@t3) {
		    if ($r{$t}) {
			push @newt3, $r{$t};
		    } else {
			push @newt3, $t;
		    }
		}
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
