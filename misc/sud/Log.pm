package ORACC::OSS2::Log;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/log_args log_hash log_node_map log_nodes/;

use warnings; use strict;

use open 'utf8';
use Data::Dumper;
my $logging = 1;

my @indent_stack = ();
my @phrasalf = ();

sub
log_hash {
    my $n = shift;
    my @n = ();
    foreach my $k (keys %{$n}) {
	my $v = $$n{$k} || '';
	push @n, "$k=$v";
    }
    join('; ', @n);
}

sub
log_init {
    if ($logging) {
	open(LOG, ">parser2.log");
    }
}

sub
log_term {
    if ($logging) {
	close(LOG);
    }
}

sub
log_args {
    print LOG @_;
}

sub
debnod {
    show_nodes('',1,'',@_);
}

sub
log_node_map {
    my($mess,$noderef,$mapref) = @_;
    my @nodes = @$noderef;
    my @map = @$mapref;
    print LOG $mess;
    for (my $i = 0; $i <= $#map; ++$i) {
	printf LOG "[%02d] ", $i;
	print LOG $map[$i] || '.';
	print LOG "\t";
	log_with_prefix("\t", $nodes[$i]);
    }
    print LOG "============================\n";
}

sub
sprint_phrase_list {
    my $all_leaves = check_leaves(@_);
    my $indent = ' ' x $indent_stack[$#indent_stack];
    my $need_indent = 0;
    my $longest_label = label_length(@_);

    if ($all_leaves) {
	my $indent_diff = 0;
	if ($#phrasalf >= 0) {
	    $indent_diff = $indent_stack[$#indent_stack]
		- length($phrasalf[$#phrasalf]);
	}
	if ($indent_diff > 0) {
	    ++$indent_diff;
	    $phrasalf[$#phrasalf] .= ' 'x$indent_diff;
	}
    }

    foreach my $node (@_) {
	next unless $$node{'type'};
	if ($$node{'type'} eq 'segment') {
	    push @phrasalf, "\n";
	    push @phrasalf, "$indent(SEG";
	    push @indent_stack, $longest_label + $indent_stack[$#indent_stack];
	    sprint_phrase_list(@{$$node{'children'}});
	    pop(@indent_stack);
	    push @phrasalf, ")";
	} elsif ($$node{'type'} eq 'phrase' || $$node{'type'} eq 'clause') {
	    if ($need_indent++) {
		push @phrasalf, "\n$indent";
	    } else {
		my $indent_diff = 0;
		if ($#phrasalf >= 0) {
		    $indent_diff = $indent_stack[$#indent_stack]
			- length($phrasalf[$#phrasalf]);
		}
		if ($indent_diff > 0) {
#		    ++$indent_diff;
		    $phrasalf[$#phrasalf] .= ' 'x$indent_diff;
		}
	    }
	    if ($#phrasalf >= 0) {
		$phrasalf[$#phrasalf] .= "($$node{'label'} ";
	    } else {
		push @phrasalf, "($$node{'label'} ";
	    }
	    push @indent_stack, $longest_label + $indent_stack[$#indent_stack];
	    sprint_phrase_list(@{$$node{'children'}});
	    pop(@indent_stack);
	    push @phrasalf, ")";
	} else {
	    my($pos,$leaf) = ($$node{'pos'},$$node{'form'});
	    $pos = '' unless $pos;
	    $leaf = '' unless $leaf;
	    if ($all_leaves) {
		;
	    } else {
		if ($need_indent++) {
		    push @phrasalf, "\n";
		    push @phrasalf, $indent;
		} else {
		    my $indent_diff = $indent_stack[$#indent_stack]
		        - length($phrasalf[$#phrasalf] || '');
		    if ($indent_diff > 0) {
#			++$indent_diff;
			$phrasalf[$#phrasalf] .= ' 'x$indent_diff;
		    }
		}
	    }
	    if ($#phrasalf >= 0) {
		$phrasalf[$#phrasalf] .= "($pos $leaf)";
	    } else {
		$pos = $pos || '';
		$leaf = $leaf || '';
		push @phrasalf, "($pos $leaf)";
	    }
	}
    }
}

sub
check_leaves {
    foreach (@_) {
	return 0 unless $$_{'type'} && $$_{'type'} eq 'leaf';
    }
    1;
}

sub
label_length {
    my $len = 0;
    foreach my $node (@_) {
	next unless $$node{'type'};
	my $nlen;
	if ($$node{'type'} eq 'segment') {
	    $nlen = 3;
	} elsif ($$node{'type'} eq 'leaf') {
#	    $nlen = length $$node{'pos'};
	    $nlen = 0;
	} else {
	    $nlen = length $$node{'label'};
	}
	$len = $nlen unless $len >= $nlen;
    }
    $len + 2;
}
sub
log_phrases {
#    print Dumper \@_;
    my $mess = shift @_;
    print LOG $mess;
    print LOG sprint_phrases(@_);
    print LOG "=====================================\n";
}

sub
sprint_phrases {
    my ($id,$pretty,$sentence) = @_;
    @indent_stack = (0);
    @phrasalf = ();
    push @indent_stack, 0;
#    sprint_phrase_list(@{$$sentence{'children'}});
    sprint_phrase_list($sentence);
    join('',@phrasalf, "\n");
}

sub
show_nodes {
    return unless $logging;
    my($mess,$deep,$prefix) = (shift,shift,shift);
    print LOG $mess;
    foreach my $n (@_) {
	my $type = $$n{'type'};
	my $what = $type eq 'leaf' ? $$n{'pos'} : $$n{'label'};
	my $form = $$n{'form'} || '';
	print LOG "${prefix}\[ $type :: $what :: $form ]\n";
	if ($deep) {
	    if ($type eq 'phrase') {
		show_nodes('',$deep,"  $prefix",@{$$n{'children'}});
	    }
	}
    }
}

sub
log_nodes2 {
    if ($logging >= 2) {
	log_nodes(@_);
    }
}
sub
log_nodes3 {
    if ($logging >= 3) {
	log_nodes(@_);
    }
}

sub
log_nodes {
    return unless $logging;
    my($mess,$deep,$prefix) = (shift,shift,shift);
    print LOG $mess;
    foreach my $n (@_) {
	next unless $n;
	my $type = $$n{'type'};
	unless ($type) {
	    warn "Phrasal:$Phrasal::error_id ($Phrasal::error_label): undefined type\n";
	    warn Dumper $n;
	    $type = 'XXX';
	    if ($$n{'label'}) {
		$$n{'label'} .= '-XXX';
	    } else {
		$$n{'label'} = 'XXX';
	    }
	}
	my $what = $type eq 'leaf' ? $$n{'pos'} : ($$n{'label'} ? $$n{'label'} : ($$n{'form'}||'[anonymous]'));
	unless (defined $what) {
	    use Data::Dumper;
	    my $nodestr = Dumper($n);
	    warn "Phrasal:$Phrasal::error_id ($Phrasal::error_label): undefined what in: $nodestr\n";
	    $what = '';
	}
	my $form = $$n{'form'} || '';
	my($file,$line,$id) = ($$n{'file'},$$n{'line'},$$n{'id'});
	my $out = "${prefix}\[ $type :: $what :: $form ]";
	my $pad = 60 - length($out);
	my $idfmt = sprintf("\#%04d",$id);
	my $deb = (' 'x$pad)."$idfmt \@$file:$line";
	print LOG "$out$deb\n";
	if ($deep) {
	    if ($type eq 'phrase' || $type eq 'clause') {
		if ($$n{'children'}) {
		    if (ref($$n{'children'}) eq 'ARRAY') {
			log_nodes('', $deep, "  $prefix", @{$$n{'children'}});
		    } else {
			print LOG "NON-ARRAY-CHILDREN\n", Dumper $$n{'children'};
		    }
		}
	    }
	}
    }
}

sub
show {
    foreach my $p (@_) {
	print sprint_phrases('',0,$p);
    }
}

sub
log_with_prefix {
    my $prefix = shift;
    foreach my $p (@_) {
	$p = sprint_phrases('',0,$p,$prefix);
	$p =~ s/\n/\n$prefix/g;
	$p =~ s/\t$//;
	print LOG $p;
    }
}

1;
