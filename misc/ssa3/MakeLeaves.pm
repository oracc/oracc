package ORACC::SSA3::MakeLeaves;
use warnings; use strict; use open ':utf8'; use utf8;
use lib '@@ORACC@@/lib';
use ORACC::OSS2::Node;
use ORACC::XML;
use ORACC::NS;
use ORACC::SSA3::Is;

use Data::Dumper;

my @taglist = qw/
    vpr1nu     NEG
    vpr1na     PROH
    vpr1ga     COH
    vpr1u      ANT
    vpr1na+    MD
    vpr1ha     MD
    vpr1bara   MD
    vpr1ca     MD
    vpr1ša     MD
    vpr1nuc    MD
    vpr1nuš    MD
    vpr2inga   CONJ
    vpr3mu     CN
    vpr3ba     CN
    vpr3V      CN
    vpr3Vmma   CN
    vpr3al     CN
    vpr4a      DAT
    vpr4ra     DAT
    vpr4na     DAT
    vpr4me     DAT
    vpr4ne     DAT
    vpr5m      P2
    vpr5e      P2
    vpr5n      P2
    vpr5b      P2
    vpr6da     COM
    vpr7m      P2
    vpr7e      P2
    vpr7n      P2
    vpr7b      P2
    vpr8ta     ABL
    vpr8ci     TER
    vpr8ši     TER
    vpr9ni     LOC
    vpr9a      LOC
    vpr9i      LT
    vpr9I      LT
    vpr10m     P1
    vpr10e     P1
    vpr10n     P1
    vpr10b     P1
    vsf1e      E
    vsf2de     DE
    vsf3en     PRO
    vsf3enden  PRO
    vsf3enzen  PRO
    vsf3ene    PRO
    vsf3ec     PRO
    vsf3eš     PRO
    vsf4a      NOM
    vsf4am     COP
    vsf4ma     ???
    vsf5ri     ???
    vsf5ece    ???
    vsf5eše    ???
    nsf1ak     GEN
    nsf1ak.ak  GEN2
    nsf2ju     POS
    nsf2ŋu     POS
    nsf2zu     POS
    nsf2ani    POS
    nsf2bi     POS
    nsf2me     POS
    nsf2zunene POS
    nsf2anene  POS
    nsf2bi     POS
    nsf3ak     GEN
    nsf4ene    PL
    nsf5ak     GEN
    nsf6ene    PL
    nsf7       ABS
    nsf70      ABS
    nsf70V     VOC
    nsf70I     INSERTP
    nsf7STOP   STOP
    nsf7e      ERG
    nsf7eE     ERG
    nsf7eL     LT
    nsf7ra     DAT
    nsf7da     COM
    nsf7ta     ABL
    nsf7ece    TER
    nsf7eše    TER
    nsf7a      LOC
    nsf7l      LT
    nsf7eV     VOC
    nsf7gin    EQU
    nsf8men    COP
    nsf8men    COP
    nsf8am     COP
    nsf8menden COP
    nsf8mezen  COP
    nsf8mec    COP
    nsf8meš    COP
    /;
my %postags = @taglist;

my %empty_hash = ();
my $empty_hash_ref = \%empty_hash;

sub make_leaf ($$$$$$$$$$);
sub make_node ($$$$$);

my $empty_node = ORACC::OSS2::Node::make_empty_node();

sub
make_leaves {
    my @leaves = ();
#    warn "make_leaves: in: @_\n";
    foreach my $pw (@_) {
#	print Dumper $$pw{'type'};
	if ($$pw{'type'} eq 'lemma') { # ${$$pw{'type'}} eq 'lemma') {
	    my @parses = @{$$pw{'parses'}};
	    my $parse = $parses[0];
	    if ($$parse{'pos'} && $$parse{'pos'} !~ /^\[?X/) {
		my @new_leaves = make_leaf($parse,$$parse{'pos'},$$pw{'wid'}, 
					   undef,undef,$$pw{'ro'},$$pw{'lnode'},'lemma','MakeLeaves',129);
		if (ORACC::SSA3::Is::is_hacked_cvn($new_leaves[0]) && is_cvn($leaves[$#leaves])) {
		    shift @new_leaves;
		}
		
		if (ORACC::SSA3::Is::is_num($new_leaves[0])) {
		    if ($$pw{'position'}) {
			${$new_leaves[0]}{'position'} = $$pw{'position'};
		    }
		}
		
		push @leaves, @new_leaves;
		my @nsf = @{$$parse{'nsf'}[1]};
		if ($#nsf) {
		    for (my $i = 1; $i <= 8; ++$i) {
			if (defined $nsf[$i]) {
			    foreach my $m (split(/\./,$nsf[$i])) {
				my $virtual = $m =~ s/^\*//;
				my $mform = $virtual ? '*' : $m;
				$mform = 'e' if $mform eq 'eE';
				push(@leaves, 
				     make_leaf($parse, $postags{"nsf$i$m"}, $$pw{'wid'}, $mform, 
					       undef, $$pw{'ro'}, $$pw{'lnode'},'nsf','MakeLeaves',151));
			    }
			}
		    }
		}
	    } else {
		push @leaves, make_leaf(undef, 'X', $$pw{'wid'}, 
					$$pw{'form'} || 'X', 
					undef, $$pw{'ro'}, $$pw{'lnode'},
					'', 'MakeLeaves', 160
		    );
	    }
	    wrap_cn(@leaves);
	} elsif ($$pw{'type'} eq 'sentence' || $$pw{'type'} eq 'clause' || $$pw{'type'} eq 'phrase') {
	    my $tag = $$pw{'node'}->getAttribute('tag') || '';
	    my $nodetype = undef;
	    if ($tag) {
		if ($tag =~ s/^C-/S-/) {
		    $nodetype = 'clause';
		} elsif ($tag =~ /^S-/) {
		    $nodetype = 'sentence';
		} else {
		    $nodetype = 'phrase';
		}
	    } else {
		$nodetype = 'phrase';
		$tag = '';
	    }
	    my @c = @{$$pw{'children'}};
	    push(@leaves, 
		 ORACC::OSS2::Node::make_node($nodetype, make_leaves(@c), $tag, 'MakeLeaves',181));
	} elsif ($$pw{'type'} eq 'nonx') {
	    push(@leaves, 
		 make_leaf(undef,'STOP',undef, undef,undef,1,undef,'leaf','MakeLeaves',186));
	} else {
	    warn "ORACC::SMA::MakeLeaves: unhandled XCL2REF node type $$pw{'type'}\n";
	}
    }

#    print "\n===========\n", Dumper(\@leaves), "\n==========\n";
#    warn "make_leaves: out: @_\n";
    [ @leaves ];
}

sub
make_leaf ($$$$$$$$$$) {
    my($parse,$pos,$wid,$form,$userdata,$ro,$lnode,$subtype,$file,$line) = @_;
    $userdata = $userdata || '';
    $parse = $empty_hash_ref unless $parse;
    my $orth = '';
    warn "make_leaf: passed undefined lnode wid=$wid; pos=$pos\n" 
	unless $lnode || !$wid || $wid eq '#N';

    $lnode = undef if $lnode && $lnode eq 'DUMMY';
    
    $orth = ${$$parse{'root'}}[1] if $$parse{'root'};
#    print STDERR "make_leaf orth=$orth\n";
    my $dsu = $$parse{'dsu'};
    my $pform = $form || '';
#    print STDERR "make_leaf pos=$pos, form=$pform\n";
    if (!defined $form) {
#	print STDERR "$pos\n";
	($pos =~ /^(.*?\[.*?\])(.*)$/) && (($pos,$form) = ($2,$1));
	if ($pos eq 'Qc') {
	    $pos = 'NU';
	    $form = atfnum2words($form);
	} elsif ($pos eq 'n') {
	    $pos = 'NU';
	}
	if ($$parse{'root'} && ${$$parse{'root'}}[2] =~ /^([rR]+)\#/) {
	    $form .= '#' . $1;
	}
	if ($pos =~ /^C?V/) {
	    if (ORACC::SSA3::Is::is_finite($parse)) {
		# we're going to build a VC-F node here and return a list consisting
		# of the leaf-and-node-sequence for the fully broken-down VC-F from
		# inside this block to short-circuit the creation of the leaf node 
	        my %p = %$parse;
		my $vpr = ${$p{'vpr'}}[1];
#		my @vpr = [ @$vpr;
#		print STDERR "vpr = ", ORACC::SSA::Display::mcat($vpr), "\n";
#		print STDERR "vpr = ", ORACC::SSA::Display::mcat_x($vpr), "\n";
		my $vsf = ${$$parse{'vsf'}}[1];
		my @vsf = @$vsf;
		my @vret = ();
		my @vleaves = ();
		for (my $i = 0; $i <= 10; ++$i) {
#		    print STDERR "vpr[$i] = ", defined $$vpr[$i] ? 'def' : 'undef', "\n";
		    next unless defined $$vpr[$i];
		    my $m = $$vpr[$i];
		    my $virtual = $m =~ s/^\*//;
		    my $mform = $virtual ? '*' : $m;
		    $mform = 'na' if $mform eq 'na+';
		    my $lpos = $postags{"vpr$i$m"};
#		    print STDERR "adding $mform as $lpos\n";
		    push @vleaves, make_leaf($parse, $lpos, $wid, $mform, undef,$ro,$lnode,'vpr','MakeLeaves',248);
		}
		my $vform = $$parse{'pos'};
		my $r2 = $$parse{'root'}[2];
		my ($rr,$root) = ($r2 =~ /^([rR]+)\#(.*?)$/);
		if ($rr) {
		    $rr = "#$rr";
		} else {
		    $rr = '';
		}
		$vform =~ s/\]C?V(?:\/[ti])?$/]/;
                $vform =~ s/^(.*?)_(?=.*?\[)//;
    		if (ORACC::SSA3::Is::is_P2($vleaves[$#vleaves])) {
		    ${$vleaves[$#vleaves]}{'pos'} = 'P1';
		}
		push @vleaves, make_leaf($parse,'V', $wid, edit_sem($vform).$rr,undef,$ro,$lnode,'lemma','MakeLeaves',263);
		for (my $i = 1; $i <= 3; ++$i) {
		    next unless defined $vsf[$i];
		    my $m = $vsf[$i];
		    my $virtual = $m =~ s/^\*//;
		    my $mform = $virtual ? '*' : $m;
		    push @vleaves, make_leaf($parse,$postags{"vsf$i$m"}, $wid, 
					     $mform,undef,$ro,$lnode,'vsf','MakeLeaves',270);
		}
		push @vret, ORACC::OSS2::Node::make_node('phrase',[ @vleaves ],'VC-F','MakeLeaves',272);
                @vleaves = ();
		for (my $i = 4; $i <= 6; ++$i) {
		    next unless defined $vsf[$i];
		    my $m = $vsf[$i];
		    my $virtual = $m =~ s/^\*//;
		    my $mform = $virtual ? '*' : $m;
		    push @vret, make_leaf($parse,$postags{"vsf$i$m"}, 
					  $wid, $mform, undef,$ro,$lnode,'vsf','MakeLeaves',280);
		}
  	        if ($#vret >= 0) {
		    return @vret;
		} else {
		    return $empty_node;
		}
	    } else {
		my $vsf = ${$$parse{'vsf'}}[1];
		my @vsf = @$vsf;
		my @vret = ();
		my @vleaves = ();
		my $vform = $$parse{'pos'};
		$vform =~ s/\]C?V(?:\/[ti])?$/]/;
		if ($pos eq 'CV') {
                    $vform =~ tr/_/ /;
		    $vform =~ s/^(.*?)\s+//;
		    my $npart = $1;
#		    push @vret, make_leaf($parse,'N',$wid,$npart,'CVN');
		}
		push @vleaves, make_leaf($parse,'V', $wid, edit_sem($vform),'CVV',$ro,$lnode,'lemma','MakeLeaves',300);
		for (my $i = 1; $i <= 3; ++$i) {
		    next unless defined $vsf[$i];
		    my $m = $vsf[$i];
		    my $virtual = $m =~ s/^\*//;
		    my $mform = $virtual ? '*' : $m;
		    push @vleaves, make_leaf($parse,$postags{"vsf$i$m"}, $wid, 
					     $mform,undef,$ro, $lnode,'vsf','MakeLeaves',307);
		}
		for (my $i = 4; $i <= 6; ++$i) {
		    next unless defined $vsf[$i];
		    my $m = $vsf[$i];
		    my $virtual = $m =~ s/^\*//;
		    my $mform = $virtual ? '*' : $m;
		    push @vleaves, make_leaf($parse,$postags{"vsf$i$m"}, $wid, 
					     $mform,undef,$ro, $lnode,'','MakeLeaves',315);
		}
		push @vret, ORACC::OSS2::Node::make_node('phrase',[ @vleaves ],'VC-N','MakeLeaves',317);
		return @vret;
	    }
       } elsif ($$parse{'pos'} && $$parse{'pos'} =~ /\]AJ/) { # ADJ
	   my $form = $$parse{'pos'};
	   $form =~ s/\].*$/]/;
	   my $vsf = @{$$parse{'vsf'}}[1];
	   my @vsf = @$vsf;
	   if (defined $vsf[4] && $vsf[4] eq 'a') {
	       return ORACC::OSS2::Node::make_node('phrase',
				[ ORACC::OSS2::Node::make_node('phrase',
					    [ make_leaf($parse, 'AJ', $wid, 
							edit_sem($form),undef, $ro, $lnode, 'lemma', 'MakeLeaves',329),
					      make_leaf($parse, $postags{"vsf4a"}, $wid,
     						       'a',undef,$ro, $lnode,'','MakeLeaves',331)],
					   'VC-N','MakeLeaves',332) ],
				'MODP','MakeLeaves',333);
           } else {
	       return ORACC::OSS2::Node::make_node('phrase',
				[ make_leaf($parse, 'AJ', $wid, 
					    edit_sem($form),undef,$ro, $lnode,'lemma','MakeLeaves',337) ],
				'MODP','MakeLeaves',338);
           }
       }
    }
    $pos = 'X' unless defined $pos;
    $pos = 'NU' if $pos eq 'n';
    $pos = 'X' if $pos eq 'u';
    $dsu = undef unless $pos =~ /^N|C?V$/;
#    print STDERR "make_leaf: dsu = $dsu\n" if $dsu;
    my %tmp = ( type=>'leaf',pos=>$pos,dsu=>$dsu,orth=>$orth,
		form=>edit_sem($form),wid=>$wid,
		userdata=>$userdata,ro=>$ro,
		lnode=>$lnode,
		file=>$file, line=>$line, id=>ORACC::OSS2::Node::nextIndex()
	);
    if ($subtype) {
	$tmp{'subtype'} = $subtype;
    }
    if ($tmp{'form'} eq 'X' && $tmp{'orth'}) {
	$tmp{'form'} = $tmp{'orth'};
    }
    if ($subtype && $subtype eq 'lemma') {
	$tmp{'parse'} = $parse if $parse;
    }
    \%tmp;
}

sub
make_leaf_insertion {
    my %tmp = ( type=>'leaf',pos=>'#INS',dsu=>'',orth=>'',
		form=>$_[1],wid=>$_[0],
		userdata=>'',ro=>1 );
    \%tmp;
}

sub
make_leaf_bracket {
    my($node,$level,$label) = @_;
    my %tmp = ();
    if ($label) {
	$tmp{'type'} = 'bracket';
	$tmp{'node'} = $node;
	$tmp{'form'} = "($level";
	$tmp{'label'} = $label;
    } else {
	$tmp{'type'} = 'bracket';
	$tmp{'node'} = $node;
	$tmp{'form'} = ")$level";
    }
    \%tmp;
}

sub
wrap_cn {
    my @ret = ();
    my $last_dsu = undef;
    my @dsu = ();
    my $cn_label = 'NC';
    foreach my $l (@_) {
	my $dsu = leaf_dsu($l);
#	print STDERR "dsu = $dsu\n" if $dsu;
	# CHECKME: do any CN's have internal morphological markers???
	if ($dsu && $dsu =~ /\]CN/ && $$l{'pos'} =~ /^[NV]$/) {
	    if ($last_dsu) {
		if (same_dsu($last_dsu,$dsu)) {
		    $last_dsu = $dsu;
		    push @dsu, $l;
		} else {
		    push @ret, make_node('phrase', [ @dsu ], $cn_label,'MakeLeaves',404);
		    @dsu = ();
		    push @dsu, $l;
		    $last_dsu = $dsu;
		}
	    } else {
		push @dsu, $l;
		$last_dsu = $dsu;
	    }
	} else {
	    if ($#dsu >= 0) {
		push @ret, make_node('phrase', [ @dsu ], $cn_label,'MakeLeaves',415);
		@dsu = ();
		$last_dsu = undef;
	    }
	    push @ret, $l;
	}
    }
    if ($#dsu >= 0) {
	push @ret, make_node('phrase', [ @dsu ], $cn_label,'MakeLeaves',423);
    }
    @ret;
}

sub
edit_sem {
    my $tmp = shift;
    return '' unless $tmp;
    $tmp =~ tr/_/ /;
    $tmp =~ s/\[\]//;
    $tmp =~ s/&quot;/\"/g;
    $tmp =~ s/\[.*?=\"(.*?)\"\]/[$1]/;
    $tmp =~ s/\[to be\s+/[/;
    $tmp =~ s/\[\(to be\)\s+/[/;
    $tmp =~ s/\[to\s+/[/;
    $tmp =~ s/\[a\s+/[/;
    $tmp;
}

sub
leaf_dsu {
    my $leaf = shift;
    if ($$leaf{'parse'}) {
	${$$leaf{'parse'}}{'dsu'};
    } else {
	'';
    }
}

1;
