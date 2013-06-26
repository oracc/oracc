package ORACC::CHI::Printer;
use warnings; use strict;

sub
render {
    my @o = @_;
    my @flat = ();
    foreach my $o (@o) {
	my($j,$childref) = @$o;
	push @flat, $j if $j;
	foreach my $c (@$childref) {
	    my($cj, $cchildref) = @$c;
	    push @flat, $cj if $cj;
	    foreach my $cc (@$cchildref) {
		my ($ccj, $ccchild) = @$cc;
		push @flat, $ccj if $ccj;
		push @flat, $ccchild;
	    }
	}
    }
    join('', @flat);
}

1;
