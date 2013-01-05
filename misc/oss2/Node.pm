package ORACC::OSS2::Node;
use lib '@@ORACC@@/lib';
use ORACC::OSS2::Log;
use Data::Dumper;

my $empty_node = make_empty_node();
my $index = 1;
my $logging = 1;

sub make_node ($$$$$);

# This iterator should not be called by the
# language-specific phrasal parsers. Its job
# is to iterate over the node tree as built
# by MakeLeaves, which means stepping into
# sub-sentences and phrases and running the
# Phrasal parser in depth-first order.
sub
iterate {
    my($error_id, $error_label, $xcl_node, $sentence, $parser) = @_;
    ($Phrasal::error_id, $Phrasal::error_label) = ($error_id, $error_label);
    log_nodes("==== iterate initial state=====\n", 1, '', @{$$sentence{'children'}});
    iterate_recursive($xcl_node, $sentence, $parser);
    log_nodes("==== iterate final state=====\n", 1, '', @{$$sentence{'children'}});
    $sentence;
}

# $node is either a top-level sentence
# or a sentence/clause/phrase node constructed
# from the XCL tree
sub
iterate_recursive {
    my($xcl_node, $entry_node, $parser) = @_;
    my $i = 0;
    my @nodes = @{$$entry_node{'children'}};
    for (my $i = 0; $i <= $#nodes; ++$i) {
	if (${$nodes[$i]}{'type'} eq 'sentence'
	    || ${$nodes[$i]}{'type'} eq 'clause'
	    || (${$nodes[$i]}{'type'} eq 'phrase'
		&& !is_terminal($nodes[$i]))) {
	    iterate_recursive($xcl_node, $nodes[$i], $parser);
	}
    }
    my $parsed_node = $parser->parse($xcl_node,$entry_node);
#    fixup_nodes($entry_node, $parsed_node);
    $parsed_node;
}

#sub
#fixup_nodes {
#    my($orig, $parsed) = @_;
#    if ($$orig{'label'} =~ /^\#/) {
##	warn "orig: $$orig{'label'}; parsed: $$parsed{'label'}\n";
#	if ($#${$parsed{'children'}} == 0) {
#	    $
#	}
#    }
#}

sub
is_terminal {
    my $n = shift;
    return $$n{'label'} =~ /^VC-/;
}

sub
nextIndex {
    $index++;
}

sub
make_node ($$$$$) {
    my($type,$children,$label,$file,$line) = @_;
    if ($type && $#$children >= 0) {
	my %mp = (type=>$type,children=>$children,file=>$file,line=>$line,id=>nextIndex());
	$mp{'label'} = $label if ($type eq 'phrase' || $type eq 'clause' || $type eq 'sentence');
	my $mpref = { %mp };
	foreach my $c (@$children) {
	    if (!ref($c)) {
#		print STDERR "child is not a ref; type=$type, label=$label\n";
	    } else {
#		$$c{'parent'} = $mpref;
	    }
	}
	$mpref;
    } else {
	# undef;
	$empty_node;
    }
}

sub
make_empty_node {
    my %mp = (type=>'X',children=>[],label=>'X','EMPTY',0);
    \%mp;
}

1;
