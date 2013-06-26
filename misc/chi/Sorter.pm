package ORACC::CHI::Sorter;
use warnings; use strict;

sub
sortuniq_split_set {
    my @o = @_;
    my %seen = ();
    my @indirects = ();
    my @new_outer;
    foreach my $o (@o) {
	my $indirect_children = $$o[1];
	my @ic_heads = ();
	foreach my $ic (@$indirect_children) {
	    my $direct_children = $$ic[1];
	    # Each $dc is a pair of strings [ JOINER , CHI ]
	    my @new_direct = ();
	    foreach my $dc (@$direct_children) {
		next if ($seen{$$dc[1]}++);
		push @new_direct, $dc;
	    }
#	    warn Dumper \@new_direct;
	    if ($#new_direct >= 0) {
		if ($#new_direct > 0) {
		    @new_direct = sort { $$a[1] cmp $$b[1] } @new_direct;
		}
		if (${$new_direct[0]}[0]) {
		    # we have lost or moved the previous initial pair
		    # which had join=undef.
		    #  1) if it is gone, do nothing
		    #  2) if it is moved, shift all the preceding
		    #     joiners one to the right
		    for (my $i = 0; $i <= $#new_direct; ++$i) {
			if (!${$new_direct[$i]}[0]) {
			    while ($i) {
				${$new_direct[$i]}[0] = ${$new_direct[$i-1]}[0];
				--$i;
			    }
			    last;
			}
		    }
		    ${$new_direct[0]}[0] = undef;
		}
		$$ic[1] = [ @new_direct ];
		push @ic_heads, [ ${$new_direct[0]}[1], $ic ];
	    }
#	    warn Dumper $$ic[1];
	}
	if ($#ic_heads >= 0) {
	    if ($#ic_heads > 0) { 
		@ic_heads = sort { $$a[0] cmp $$b[0] } @ic_heads;
	    }
	    my @new_indirect = map { $$_[1] } @ic_heads;
	    if (${$new_indirect[0]}[0]) {
		# we have lost or moved the previous initial pair
		# which had join=undef.
		#  1) if it is gone, do nothing
		#  2) if it is moved, shift all the preceding
		#     joiners one to the right
		for (my $i = 0; $i <= $#new_indirect; ++$i) {
		    if (!${$new_indirect[$i]}[0]) {
			while ($i) {
			    ${$new_indirect[$i]}[0] = ${$new_indirect[$i-1]}[0];
			    --$i;
			}
			last;
		    }
		}
		${$new_indirect[0]}[0] = undef;
		$$o[1] = [ @new_indirect ];
	    }
	    push @new_outer, $o;
	} else {
	    # don't push because we ended up empty
	}
    }
    @new_outer;
}

sub
sortuniq_direct {
    my @set = @_;
    if ($#set == 0 && $set[0] =~ /\./) {
	@set = split(/\./,$set[0]);
    }
    my %set = ();
    @set{ @set } = ();
    join('.', sort keys %set);
}

1;
