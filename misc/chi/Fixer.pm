package ORACC::CHI::Fixer;
use warnings; use strict;

sub
find_prev {
    my($i,@j) = @_;
    if ($i > 1) {
	if ($j[$i-1] =~ /^\(?\+/) {
	    return $i-2;
	}
    }
    return -1;
}

sub
fix_joins {
    my @j = @_;
    for (my $i = 1; $i <= $#j; ++$i) {
	if ($j[$i] =~ /^\s*\d(?!N)/) {
	    my $prev = find_prev($i, @j);
	    if ($prev >= 0) {
		my ($prefix) = ($j[$prev] =~ /^([^0-9]+)/);
		if ($prefix) {
		    $prefix =~ s/\s*$//;
		    $j[$i] =~ s/^\s*//;
		    warn "repairing $j[$i] as $prefix $j[$i]\n";
		    $j[$i] = "$prefix $j[$i]";
		    1 while $j[$i] =~ s/  / /;
		} else {
		    warn "unable to repair $j[$i]; $j[$prev] has no prefix\n";
		}
	    } else {
		warn "unable to repair item #$i=$j[$i]; bad sequence in '@j'\n";
	    }
	} elsif ($j[$i] =~ /^\s*[a-z]\s*$/) {
	    my $prev = find_prev($i, @j);
	    if ($prev >= 0) {
		my $letter = $j[$i];
		$letter =~ s/^\s*(.)\s*$/$1/;
		my $prefix = $j[$prev];
		$prefix =~ s/[a-z]\s*$//;
		$j[$i] = "$prefix$letter";
	    } else {
		warn "unable to repair item #$i=$j[$i]; bad sequence in '@j'\n";
	    }
	}
    }
    @j;
}

1;
