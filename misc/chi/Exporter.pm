package ORACC::CHI::Render;
use warnings; use strict;

sub
un_chi {
    my @bits = split(/(\.+-?)/, $_[0]);
    my @res = ();
    foreach my $b (@bits) {
	if ($b =~ /^\./) {
	    my $q = (($b =~ s/-$//) ? "?" : "");
	    if ($b eq '...') {
		push @res, "\&$q";
	    } elsif ($b eq '..') {
		push @res, "(+)$q";
	    } else {
		push @res, "+$q";
	    }
	} else {
	    my($p,$n) = ($b =~ /^(.*?)_(.*?)$/);
	    if ($p =~ s/^N(\d+)/$1N/) {
		if ($p =~ s/NT/N-T/) {
		    push @res, "$p$n";
		} else {
		    push @res, "$p $n";
		}
	    } else {
		push @res, "$p $n";
	    }
	}
    }
    join(' ', @res);
}

1;
