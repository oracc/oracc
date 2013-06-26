package ORACC::CHI::Splitter;
use warnings; use strict;

sub
count_frags {
    my $x = shift;
    $x =~ s/\.+-?/./g;
    $x =~ tr/././;
}

sub
join_set {
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

sub
split_direct {
    my $joiner = undef;
    my @direct = ();
    foreach my $direct (split(/(\.-?)/, $_[0])) {
	if ($direct =~ /^\./) {
	    $joiner = $direct;
	} else {
	    push(@direct, [ $joiner, $direct ]);
	}
    }
    @direct;
}

sub
split_indirect {
    my $joiner = undef;
    my @indirect = ();
    foreach my $indirect (split(/(\.\.-?)/, $_[0])) {
	if ($indirect =~ /^\.\./) {
	    $joiner = $indirect;
	} else {
	    push(@indirect, [ $joiner,
			      [ split_direct($indirect) ]
		 ]);
	}
    }
    @indirect;
}

sub
split_set {
    my @outer = ();
    my $joiner = undef;
    foreach my $outer (split(/(\.\.\.-?)/, $_[0])) {
	if ($outer =~ /^\.\.\./) {
	    $joiner = $outer;
	} else {
	    push(@outer, [ $joiner,
			   [ split_indirect($outer) ]
		 ]);
	}
    }
    @outer;
}

# Input must have joiners formatted as follows:
#
# ' + ' , ' +? '
# ' & ' , ' &? '
# ' (+) ', ' (+)? '
#
sub
split_string {
#    split(/\s+\(?[+&][\)?!*]*\s+/, $_[0]);
    split(/\s+(\(?[+&]\)?\??)\s+/, $_[0]);
}

1;
