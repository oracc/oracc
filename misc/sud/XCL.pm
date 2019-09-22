package ORACC::OSS2::XCL;
use warnings; use strict;
use lib '@@ORACC@@/lib';
use ORACC::XML;
use ORACC::OSS2::Log;
use ORACC::SSA4::MakeLeaves;
use Data::Dumper;

my $discourse;
my @phrasalf = ();
my $PQ_id;
my $sid = 0;
my $status = 0;
my $xcl_uri = 'http://oracc.org/ns/xcl/1.0';
my $xcldoc = undef;
my $xclDocument = undef;

sub
xcl_print_phrases {
    my ($xcl,$id,$sentence) = @_;
    $PQ_id = $id;
    @phrasalf = ();
    
#    open(L, ">XCL-$sid.log"); ++$sid;
#    use Data::Dumper; 
#    print L Dumper $sentence;
#    close(L);

#    my $xp = createNode($xcl, 'c', 'sentence');  ## FIXME: use $xcl's type

    $xclDocument = $xcl->getOwnerDocument();

    log_args("===\$sentence passed to xclify_phrase_list===\n", Dumper($sentence),"====\n")
	if $ORACC::OSS2::verbose > 1;
    my $xp = xclify_phrase_list(undef, $sentence);
    if ($xp->getAttribute('type')) {
	if ($xp->getAttribute('type') eq 'clause'
	    && $xp->getAttribute('tag') =~ /^S-/) {
	    $xp->setAttribute('type', "sentence");
	} elsif ($xp->getAttribute('type' eq 'frag')) {
	    $xp->setAttribute('type', "frag");
	    $xp->setAttribute('tag', "FRAG");
	}
    }

    rewrite_d_nodes($xp);

#    print STDERR $xp->toString(1);

    $xp;

#    warn Dumper $xp;
#    return;
#    join('',@phrasalf);
}

sub
xclify_phrase_list {
    my $parent = shift;
    my $newNode = undef;
    foreach my $node (@_) {
	if ($$node{'parse'}) {
#	    $$node{'subtype'} = 'lemma';
	    delete $$node{'parse'};
	} elsif ($$node{'lnode'}) {
	    delete $$node{'lnode'};
	    delete $$node{'wid'};
	}
	delete $$node{'ro'};

	$newNode = createNode($parent, 'c', $$node{'type'} || 'unknown', $$node{'subtype'}, $$node{'label'} || $$node{'pos'}, $$node{'form'});
	if ($$node{'lnode'}) {
	    my @preceding_d_nodes = ();
	    my @following_d_nodes = ();
	    for (my $rover = $$node{'lnode'}->previousSibling(); $rover; $rover = $rover->previousSibling()) {
		if ($rover->nodeName() eq 'd') {
		    push @preceding_d_nodes, $rover;
		} else {
		    last;
		}
	    }
	    for (my $rover = $$node{'lnode'}->nextSibling(); $rover; $rover = $rover->nextSibling()) {
		if ($rover->nodeName() eq 'd') {
		    push @following_d_nodes, $rover;
		} else {
		    last;
		}
	    }
	    foreach my $n (@preceding_d_nodes) {
		$newNode->appendChild($n);
	    }
	    $newNode->appendChild($$node{'lnode'});
	    foreach my $n (@following_d_nodes) {
		$newNode->appendChild($n);
	    }
	}
	$parent->appendChild($newNode) if $parent;

	if ($#{$$node{'children'}} >= 0) {
	    xclify_phrase_list($newNode, @{$$node{'children'}});
	} else {
	    delete $$node{'children'};
	}
    }
    $newNode;
}

sub
createNode {
    my($refNode, $nodeName, $nodeType, $nodeSubType, $nodeLabel, $nodeForm) = @_;
    my $newNode = $xclDocument->createElementNS($xcl_uri,
						$nodeName);
    $newNode->setAttribute('type', $nodeType);
    $newNode->setAttribute('sub', $nodeSubType) if $nodeSubType;
    $newNode->setAttribute('tag', cleanLabel($nodeLabel)) if $nodeLabel;
    $newNode->setAttribute('txt', $nodeForm) if $nodeForm;
    $newNode;
}

sub
cleanLabel {
    my $l = shift;
    $l =~ s/-POS//;
    $l =~ s/-MOD//;
    $l;
}

sub
rewrite_d_nodes {
    my $top = shift;
    my @l = tags($top,$xcl_uri,'l');
    foreach my $n (@l) {
	if ($n->previousSibling() && $n->previousSibling()->localName() eq 'd') {
	    my ($parent,$ref,$before) = findPreviousInsertPoint($n);
	    if ($parent) {
		my @preceding_d_nodes = ();
		for (my $rover = $n->previousSibling(); $rover; $rover = $rover->previousSibling()) {
		    if ($rover->nodeName() eq 'd') {
			push @preceding_d_nodes, $rover;
		    } else {
			last;
		    }
		}
		foreach my $n (@preceding_d_nodes) {
		    if ($before) {
			$parent->insertBefore($n->parentNode()->removeChild($n), $parent->firstChild());
		    } else {
			$parent->insertAfter($n->parentNode()->removeChild($n), $ref || undef);
		    }
		}
	    }
	}
	if ($n->nextSibling() && $n->nextSibling()->localName() eq 'd') {
	    my($parent,$ref) = findFollowingInsertPoint($n);
	    if ($parent) {
		my @following_d_nodes = ();
		for (my $rover = $n->nextSibling(); $rover; $rover = $rover->nextSibling()) {
		    if ($rover->nodeName() eq 'd') {
			push @following_d_nodes, $rover;
		    } else {
			last;
		    }
		}
		foreach my $n (@following_d_nodes) {
		    $parent->insertBefore($n->parentNode()->removeChild($n), $ref || undef);
		}
	    }
	}
    }
}

# d nodes that come before a lemma must be moved up in the tree to
# a point between nodes that contain different words.
sub
hasLemmaDescendant {
    my $n = shift;
    foreach my $c ($n->childNodes()) {
	if ($c->hasAttribute('type') && $c->getAttribute('type') eq 'leaf') {
	    if ($c->hasAttribute('sub') && $c->getAttribute('sub') eq 'lemma') {
#		warn "hasLemmaDescendent: found lemma at $c\n";
		return 1;
	    }
	} else {
	    if (hasLemmaDescendant($c)) {
		return 1;
	    }
	}
    }
}

# In both Previous and Following cases, the insert point we return
# must be the highest node in the phrase that contains the current 
# lemma which does not itself contain another lemma.

sub
findFollowingInsertPoint {
    my $n = shift;
    my $p = $n;
    while (1) {
	# Step up in the tree
	$p = $p->parentNode();
	# Find the first following sibling that has
	# a lemma descendant
	my $f = followingSiblingWithLemma($p);
	# If there is one, then it is the node before which the 'd' nodes
	# should be inserted
	if ($f) {
	    return ($p->parentNode() , $f);
	}
	# If there isn't, see if there is another parent ELEMENT; if so, try again
	# if not, we have to insert after the last sibling of the current parent
	unless ($p->parentNode() && $p->parentNode->isa('XML::LibXML::Element')) {
	    return ($p, undef);
	}
    }
    (undef,undef);
}

sub
followingSiblingWithLemma {
    my $n = shift;
    $n = $n->nextSibling();
    while ($n) {
	if (hasLemmaDescendant($n)) {
	    return $n;
	} else {
	    $n = $n->nextSibling();
	}
    }
}

sub
findPreviousInsertPoint {
    my $n = shift;
    my $p = $n;
    while (1) {
	# Step up in the tree
	$p = $p->parentNode();
	# Find the first previous sibling that has
	# a lemma descendant
	my $f = previousSiblingWithLemma($p);
	# If there is one, then it is the node after which the 'd' nodes
	# should be inserted
	if ($f) {
	    return ($p->parentNode() , $f);
	}
	# If there isn't, see if there is another parent ELEMENT; if so, try again
	# if not, we have to insert after the last sibling of the current parent
	unless ($p->parentNode() && $p->parentNode->isa('XML::LibXML::Element')) {
	    return ($p, undef, 1);
	}
    }
    (undef, undef);
}

sub
previousSiblingWithLemma {
    my $n = shift;
    $n = $n->previousSibling();
    while ($n) {
	if (hasLemmaDescendant($n)) {
	    return $n;
	} else {
	    $n = $n->previousSibling();
	}
    }
}

#########################################################################

sub
load_xcl_refs {
    my $xcl = shift;
    $xcldoc = load_xml($xcl);

    my $xcltop = $xcldoc->getDocumentElement();
    if ($xcltop->localName() eq 'xtf') {
	my @c = $xcltop->childNodes();
	for (my $i = 0; $i <= $#c; ++$i) {
	    if ($c[$i]->isa('XML::LibXML::Element')) {
		$xcltop = $c[$i];
		last;
	    }
	}
    }

    my $xid = $xcltop->getAttribute('ref');
    my $t = $xcltop->getAttribute('type');
    my @refs = ();
    foreach my $s (tags($xcltop,'http://oracc.org/ns/xcl/1.0', 'c')) {
	next unless $s->getAttribute('type') eq 'sentence';
	my @c = $s->childNodes();
	if ($#c == 0 && !$c[0]->hasAttribute('tag')) {
	    my @pc = $c[0]->childNodes();
	    $s->removeChildNodes();
	    foreach (@pc) {
		$s->addChild($_);
	    }
	}
	my @r = xcl2ref($xcldoc,$s);

	# if xcl2ref is passed a singleton sentence node 
	# it must be a fatal error to get more than one node back
	die "r > 0\n" if $#r > 0;

	log_args("===refs===\n", Dumper(\@r), "===\n") if $ORACC::OSS2::verbose > 1;
	exit 0 if $ORACC::OSS2::refs_only;

	# This can only return a singleton node because xcl2ref is now
	# passed sentence-nodes from this routine
	my $wrapped_r = ORACC::SSA4::MakeLeaves::make_leaves(@r);

	my $r = [ xid($s) , $s->getAttribute('label') , @$wrapped_r ];

	log_args("===leaves===\n", Dumper($r), "===\n") if $ORACC::OSS2::verbose > 1;
	exit 0 if $ORACC::OSS2::leaves_only;

	log_args("===after make_leaves===\n", Dumper($r) , "=====\n")
	    if $ORACC::OSS2::verbose > 1;

	push @refs, [ $s , $r ] if $#$r > 0;
    }

    ($xcldoc , @refs);
}

sub
xcl2ref {
#    my ($id,$label,@n) = @_;
    my($xclnode,@n) = @_;
#    warn "oss.plx: attempt to call xcl2ref on undefined node\n" and return unless $sent;
#    my $id = xid($sent);
#    my $label = $sent->getAttribute('label');
#    my @n = $sent->childNodes();
    my @w = ();
    my $position = undef;
    foreach my $node (@n) {
#    foreach my $node ($sent) {
	my $xid = $node->getAttribute('ref');
	my $lname = $node->localName();
	if ($lname eq 'l') {
#	    next if $discourse;
	    my %w = ();
	    my $pos = make_pos($node);
	    $w{'type'} = 'lemma';
	    $w{'parses'} = [ ORACC::SMA::ParseManual::mk_parse_from_pos($pos) ];
	    $w{'ro'} = 1;
	    $w{'lnode'} = $node;
	    $w{'wid'} = $xid;
	    push @w, { %w };
	} elsif ($lname eq 'll') {
	    foreach my $lc ($node->childNodes()) {
		$node = $lc and last
		    if $lc->isa('XML::LibXML::Element') and $lc->localName() eq 'l';
	    }
	    my %w = ();
	    my $pos = make_pos($node);
	    $w{'type'} = 'lemma';
	    $w{'parses'} = [ ORACC::SMA::ParseManual::mk_parse_from_pos($pos) ];
	    $w{'ro'} = 1;
	    $w{'wid'} = $xid;
	    push @w, { %w };
	} elsif ($lname eq 'd') {
	    my $dtype = $node->getAttribute('dtype') || $node->getAttribute('type');
	    if ($dtype eq 'break') {
		push @w, { 
		    type=>'break',
		    w=>      { node=>$node,
			       wid =>$xid,
			       form=>'' },
		    parses=> [ ],
		    break=>  1,
		};
		undef $position;
	    } elsif ($dtype eq 'div-start' || $dtype eq 'div-end') {
		# ignore for now
	    } elsif ($dtype eq 'line-start') {
		# set the 'li' flag on the next node and the 'lf' flag
		# on the preceding node
		if ($#w >= 0) {
		    $w[$#w]{'position'} = 'final';
		}
		$position = 'initial';
	    } elsif ($dtype eq 'locator') {
		# do nothing; line-discontinuities output the required
		# potential caesura information
	    } elsif ($dtype eq 'discourse') {
		my $subtype = $node->getAttribute('subtype');
		if ($subtype eq 'body') {
		    $discourse = 0;
		} else {
		    $discourse = 1;
		}
	    } elsif ($dtype eq 'nonx') {
		my $nonx = $xcldoc->getElementById($xid);
		my $state = $nonx->getAttribute('state');
		if ($state && $state eq 'missing') {
		    push @w, { 
			type=>'nonx',
			w=>      { node=>$nonx,
				   wid =>$xid,
				   form=>'' },
			parses=> [ ],
			break=>  1,
			ref=>    $nonx->getAttribute('ref'),
			lnode=>  $nonx
		    }
		} else {
		    # FIXME: REVIEW OTHER CASES
		}
	    } else {
		bad("unhandled dtype '$dtype'");
	    }
	} elsif ($lname eq 'c'
		 && $node->getAttribute('type') =~ /^(?:sentence|clause|phrase)$/) {
	    my @res = xcl2ref($xclnode, $node->childNodes());
	    # in sub-nodes of xcl2ref we ignore the first two
	    # returned arguments, the ID and LABEL
	    push @w, {
		type=>$node->getAttribute('type'),
		node=>$node,
		children=> [ @res ]
	    };
	} else {
	    bad("unhandled node $lname");
	}
    }
    @w;
}

sub
bad {
    warn('oss.plx: ', @_, "\n");
    ++$status;
}

sub
getCFGW {
    my $f = shift;
    my $cf = $f->getAttribute('cf');
    my $gw = $f->getAttribute('gw');
    if ($cf && $gw) {
	if ($f->getAttribute('epos') &&
	    $f->getAttribute('epos') ne $f->getAttribute('pos')) {
	    my $s = $f->getAttribute('sense');
	    $s =~ s/,.*$//;
	    $s =~ s/^to(?:\s+\(?be\)?\s+)//;
	    "$cf\[$s\]";
	} else {
	    "$cf\[$gw\]";
	}
    } else {
	$f->getAttribute('epos') || $f->getAttribute('pos');
    }
}

sub
getmorph {
    my $f = shift;
    $f->getAttribute('morph') || '~';
}

sub
form_of {
    my $n = shift;
    foreach my $c ($n->childNodes()) {
	return $c if $c->isa('XML::LibXML::Element') && $c->localName() eq 'f';
    }
    undef;
}

sub
make_pos {
    my $n = shift;
    my $f = form_of($n);
    my $cfgw = getCFGW($f);
    my $pos = $f->getAttribute('epos') || $f->getAttribute('pos') || 'X';
    if (!$cfgw || $cfgw eq 'n') {
	if ($pos eq 'n') {
	    my $form = $f->getAttribute('base') || $f->getAttribute('form');
	    return $form.'[]n#'.getmorph($f);
	} else {
	    my $form = $f->getAttribute('form');
	    return $form.'[]X#'.getmorph($f);
	}
    } else {
	$cfgw .= '[]' unless $cfgw =~ /\]$/;
	my $base = $f->getAttribute('base') || $f->getAttribute('cf');
	my $morph = ($f->getAttribute('morph') || $base || $f->getAttribute('form'));
	if ($base) {
	    $morph = getmorph($f);
	    $morph =~ s/~/$base/ if $morph;
	}
	"$cfgw$pos#$morph";
    }
}

1;
