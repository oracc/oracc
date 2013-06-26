package ORACC::CHI::Merger;
use warnings; use strict;
use Data::Dumper;

sub
merge {
    my($what,$into,$where) = @_;
    my($outer,$indirect) = (0,0);
    if ($where) {
	my $ncolon = ($where =~ tr/:/:/);
	if ($ncolon > 1) {
	    die "ORACC::CHI::Merger: 'where' index must be single or double\n";
	} elsif (1 == $ncolon) {
	    ($outer,$indirect) = split(/:/,$where);
	} else {
	    if ($what =~ /^\.\.\./) {
		$outer = $where;
	    } else {
		die "ORACC::CHI::Merger: 'where' index must be double unless merging '...'\n";
	    }
	}
    }
    my @outer = ();
    if ($into =~ /\.\.\./) {
	@outer = split(/(\.\.\.-?)/, $into);
    } else {
	@outer = ($into);
    }
    $outer = $#outer+1 unless $outer <= $#outer;
    if ($what =~ /^\.\.\./) {
	return merge_outer($what, $outer, undef, @outer);
    } elsif ($what =~ /^\.\./) {
	return merge_outer($what, $outer, $indirect, @outer);
    } else {
	return merge_outer($what, $outer, $indirect, @outer);
    }
}

sub
merge_outer {
    my($what, $outer, $indirect, @outer) = @_;
    if ($outer == 0) {
	if (defined $indirect) {
	    return join('', 
			merge_indirect($what, $indirect, shift @outer),
			@outer);
	} else {
	    $what =~ s/^(\.\.\.-?)//;
	    my $j = $1;
	    $outer[0] =~ s/^/$j/;
	    return join('',$what,@outer);
	}
    } else {
	my @new_outer = ();
	$outer = $#outer unless $outer < $#outer;
	my $nth = 0;
	my $done = 0;
	for (my $i = 0; $i <= $#outer; ++$i) {
	    if ($outer[$i] !~ /^\./) {
		if ($nth == $outer) {
		    if (defined $indirect) {
			push(@new_outer,
			     merge_indirect($what, $indirect, $outer[$i]));
		    } else {
			push @new_outer, $outer[$i];
			push @new_outer, $what;
		    }
		    $done = 1;
		} else {
		    push @new_outer, $outer[$i];
		}
		++$nth;
	    } else {
		push @new_outer, $outer[$i];
	    }
	}
	push @new_outer, $what unless $done;
	return join('',@new_outer);
    }
}

sub
merge_indirect {
    my($what, $indirect, $into) = @_;
    # unlike outers, indirects are subject to sorting
    # so we can just stick this on the end if we are adding an
    # entire indirect chunk
    if ($what =~ /^\.\./) {
	return $into . $what;
    }
    my $nth = 0;
    my @indirect = ();
    my @new_indirect = ();
    my $done = 0;
    if ($into =~ /\.\./) {
	@indirect = split(/(\.\.-?)/, $into);
    } else {
	@indirect = ($into);
    }
    $indirect = $#indirect+1 unless $indirect <= $#indirect;

    warn Dumper @indirect;

    if ($indirect == 0) {
	return join('', 
		    merge_direct($what, shift @indirect),
		    @indirect);
    } else {
	for (my $i = 0; $i <= $#indirect; ++$i) {
	    if ($indirect[$i] !~ /^\./) {
		if ($nth == $indirect) {
		    push @new_indirect, merge_direct($what, $indirect[$i]);
		    $done = 1;
		} else {
		    push @new_indirect, $indirect[$i];
		}
		++$nth;
	    } else {
		push @new_indirect, $indirect[$i];
	    }
	}
    }
    push(@new_indirect, merge_direct($what,pop(@new_indirect)))
	unless $done;
    return join('', @new_indirect);
}

sub
merge_direct {
    my($what, $indirect) = @_;
    return $indirect . $what;
}

1;
