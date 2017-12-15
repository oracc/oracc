package ORACC::JSON;
use warnings; use strict; use utf8;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use DateTime;

##############################################
#
# JSON.pm is an infrastructure for turning XML into JSON
# intelligently.  It takes a configuration structure mapping
# XML element names to action definitions to create names/values
# and to structure the JSON as arrays or hashes.
#
# The config structure may have these members:
#
# nam
#   #ignore -- no name is generated
#   a string literal -- name is the literal
#   an attribute, with '@' prefix -- name is value of @attr
#   text() -- name is text content of node
#
# val
#   #ignore -- no value is generated
#   a string literal -- value is the literal
#   an attribute, with '@' prefix -- value is value of @attr
#   text() -- value is text content of node
#   [ or { -- value is an array or hash
#   hook() -- value is the value returned from the hook function
#
# chld
#   [ or { -- children are wrapped in an array or hash
#
# att
#   empty string -- all attributes are emitted as properties
#   string -- space separated list of attributes
#             -- if first char is '-', named atts are dropped
#             -- else named atts are emitted
#   0 -- (digit zero) -- no attributes are emitted
#
# text
#   string -- the text content of the node is treated as the
#             value of a named property named 'string'; useful
#             if an element has attributes wrapped in an object
#             and you want to treat the text content as if it
#             were just another attribute
#
# hook
#   a function reference (e.g., \&textfrag).  Run after the name/value 
#   are set or, if val==hook(), to provide the value
#
# Example (from cbd-json.plx):
# 
#sub
#glossary_howtos {
#    $howto{'cbd_entries'} = {
#	nam=>"entries", val=>'[',
#    };
#    $howto{'cbd_entry'} = {
#	type=>"{",
#	nam=>'headword',val=>'@n',
#	att=>'-n'
#    };
#    $howto{'cbd_cf'} = { nam=>'cf',val=>'text()' };
#    $howto{'cbd_gw'} = { nam=>'gw',val=>'text()' };
#    $howto{'cbd_pos'} = { nam=>'pos',val=>'text()' };
#    $howto{'cbd_epos'} = { nam=>'epos',val=>'text()' };
#    $howto{'cbd_mng'} = { nam=>'mng',val=>'text()' };
#    $howto{'cbd_forms'} = { nam=>'forms',val=>'[' };
#    $howto{'cbd_form'} = { type=>"{",nam=>'type',val=>'form',att=>'' };
#    $howto{'cbd_t'} = { type=>'#ignore' };
#    $howto{'cbd_cof-form-norm'} = { nam=>'cof-form-norm',val=>'text()',att=>'' };
#    $howto{'cbd_norms'} = { nam=>'norms',val=>'[' };
#    $howto{'cbd_norm'} = { type=>"{",nam=>'#ignore',val=>'#ignore',att=>'' };
#    $howto{'cbd_n'} = { nam=>'n',val=>'text()',att=>'' };
#    $howto{'cbd_f'} = { type=>"{",nam=>'type',val=>'normform',att=>'' };
#    $howto{'cbd_bases'} = { nam=>'bases',val=>'[' };
#    $howto{'cbd_base'} = { type=>"{",nam=>'type',val=>'base',att=>'' };
#    $howto{'cbd_morphs'} = { nam=>'morphs',val=>'[' };
#    $howto{'cbd_morph'} = { type=>"{",nam=>'type',val=>'morph',att=>'' };
#    $howto{'cbd_conts'} = { nam=>'conts',val=>'[' };
#    $howto{'cbd_cont'} = { type=>"{",nam=>'type',val=>'cont',att=>'' };
#    $howto{'cbd_compound'} = { nam=>'compound',val=>'[' };
#    $howto{'cbd_cpd'} = { type=>"{",nam=>'type',val=>'cpd',att=>'' };
#    $howto{'cbd_senses'} = { nam=>'senses',val=>'[' };
#    $howto{'cbd_sense'} = { type=>"{",nam=>'type',val=>'sense',att=>'' };
#    $howto{'cbd_sigs'} = { nam=>'sigs',val=>'[' };
#    $howto{'cbd_sig'} = { type=>"{",nam=>'type',val=>'sig',att=>'' };
#    $howto{'cbd_cof-data'} = { nam=>'cof-data',val=>'{',att=>'' };
#    $howto{'cbd_cof-head'} = { nam=>'head',val=>'text()',att=>'0' };
#    $howto{'cbd_cof-tail'} = { nam=>'tail',val=>'{',att=>'',text=>'sig' };
#
#    $howto{'xis_xisses'} = {
#	nam=>'instances',
#	val=>'{'
#    };
#    $howto{'xis_xis'} = {
#	nam=>'@xml:id',
#	val=>"[",
#	att=>'0',
#    };
#    $howto{'xis_r'} = { nam=>'#ignore', val=>'text()' };
#    $howto{'xis_periods'} = { type=>'#ignore' };
#
#    ORACC::JSON::setHowTos(%howto);
#}
#

my %attmap = ();
my $formtop = 0;
my %howtos = ();
my $need_comma = 0;
my @stack = ();
my %trigger_after = ();
my %trigger_before = ();

sub
setAttMap {
    %attmap = @_ ;
}

sub
setHowTos {
    %howtos = @_ ;
    foreach my $k (keys %howtos) {
	my $h = $howtos{$k};
	if (ref($h) eq 'HASH' && $$h{'trigger'}) {
	    my $hook = $$h{'hook'};
	    my ($when,$attr) = ($$h{'trigger'} =~ /^(.*?)\@(.*?)$/);
	    if ($when eq 'after') {
		$trigger_after{"$k:$attr"} = $hook;
	    } else {
		$trigger_before{"$k:$attr"} = $hook;
	    }
	}
    }
#    use Data::Dumper; print STDERR Dumper(\%trigger_after); exit 0;
}

sub
hasElementChildren {
    my $n = shift;
    foreach my $c ($n->childNodes()) {
	return 1 if $c->isa('XML::LibXML::Element');
    }
    return 0;
}

sub
iterate {
    my ($n,$top) = @_;
    my $ecount = 0;

    # if ignore and recurse=no we get a positive 
    # return from node_start and we skip recursion
    my $norecurse = node_start($n,$top);

    unless ($norecurse) {
	foreach my $c ($n->childNodes()) {
	    my $isa = ref($c);
	    # the input will be Oracc data, no whitespace, no mixed content
	    if ($isa eq 'XML::LibXML::Element') {
		iterate($c);
	    }
	}
    }
    
    node_end($n);
}

sub
node_start {
    my ($n,$top) = @_;
    my $nm = $n->nodeName();
    my $context = $stack[$#stack];

  AUTO_RETRY:
    
    my $howto = $howtos{$nm};

    my $type_closer = '';
    my $val_closer = '';
    my $chld_closer = '';
    
    if ($howto) {

	if ($$howto{'formchild'} && !$top) {
	    my $xid = xid($n);
#	    warn "formchild activated at node $xid; context=$context\n";
	    $howto = $$howto{'formchild'};
	}

	my $type_how = $$howto{'type'} || '';

	if ($type_how eq '#ignore') {
	    push @stack, '';
	    if ($$howto{'recurse'} && $$howto{'recurse'} eq 'no') {
		return 1;
	    } else {
		return 0;
	    }
	}

	# Compute val first so we can skip comma if we have an empty val in skipempty mode
	my $val = undef;
	my $val_need_comma = 0;
	my $val_how = $$howto{'val'};
	unless ($val_how eq '#ignore') {
	    # Value must always push closer information 
	    # onto @stack unless $val_how = #ignore
	    if ($val_how =~ /^\@(\S+)/) {
		my $aname = $1;
		my $attr = undef;
		if ($attr = $n->getAttribute($aname)) {
		    $val = $attr;
		    $val_need_comma = 1;
		} else {
		    warn "element $nm wants value from absent attribute $aname\n";
		}
		$val_closer = '';
	    } elsif ($val_how =~ /^[\[\{]$/) {
		$val = '#object';
	    } elsif ($val_how =~ /text()/) {
		foreach my $c ($n->childNodes()) {
		    if ($c->isa('XML::LibXML::Text')) {
			$val = $c->textContent();
		    }
		}
		$val = '' unless $val;
		$val_need_comma = 1;
		$val_closer = '';
		if (!$val && $$howto{'chld'} && hasElementChildren($n)) {
		    $val = '#chld';
		}
	    } elsif ($val_how eq 'hook()') {
		if (defined $$howto{'hook'}) {
		    my $h = $$howto{'hook'};
		    $val = &$h($n);
		} else {
		    warn "element $nm wants value from absent hook()\n";
		}
		$val = '' unless $val;
		$val_need_comma = 1;
		$val_closer = '';
	    } elsif (length $val_how) {
		$val = $val_how;
		$val_need_comma = 1;
		$val_closer = '';
	    } else {
		$val = '{';
		$val_closer = '}';
	    }
	}

	if (!$val && $howtos{'#skipempty'}) {
	    push @stack, '';
	    return 1
		if ($$howto{'recurse'} && $$howto{'recurse'} eq 'no');
	    return 0;
	}

	my $nam = $nm;
	my $nam_how = $$howto{'nam'};
	if ($nam_how =~ /^\@(\S+)/) {
	    my $aname = $1;
	    my $attr = undef;
	    if ($attr = $n->getAttribute($aname)) {
		$nam = $attr;
	    }
	} elsif ($nam_how eq 'text()') {
	    foreach my $c ($n->childNodes()) {
		if ($c->isa('XML::LibXML::Text')) {
		    $nam = $c->textContent();
		}
	    }
	    $nam = '' unless $nam;
	} elsif (length $nam_how) {
	    $nam = $nam_how;
	} else {
	    $nam = $nm;
	}

	if ($need_comma) {
	    print ",\n";
	    $need_comma = 0;
	}

	if ($type_how) {
	    if ($$howto{'object_force_nam'}) {
		if ($$howto{'object_force_nam'} eq 'yes'
		    && $context =~ /^}/) {
		    print "\"$nam\": ";
		}
	    }
	    
	    $type_closer = closer_of($type_how);
	    print $type_how, "\n";
	}
	
	if ($val && $val eq '#object') {
	    unless ($nam_how eq '#ignore') {
		jprint($nam);
		print ": ";
	    }
	    unless ($val_how eq '#ignore') {
		print $val_how;
		$val_closer = closer_of($val_how);
		if ($$howto{'text'}) {
		    print "\n";
		    jprint($$howto{'text'});
		    print ": ";
		    jprint($n->textContent());
		    $need_comma = 1;
		}
	    }
	} else {
	    # naming is ignored when there's a chld key in howto
	    unless ($val && $val eq '#chld') {
		if ($val || !$howtos{'#skipempty'}) {
		    unless ($nam_how eq '#ignore') {
			jprint($nam);
			print ": ";
		    }
		    jprint($val) 
			unless $val_how eq '#ignore' || $val eq '{';
		    $need_comma = $val_need_comma;
		} else {
		    $need_comma = 0;
		}
	    }
	}

	if ($$howto{'hook'} && !$$howto{'trigger'} && !$val_how eq 'hook()') {
	    my $h = $$howto{'hook'};
	    my $prop = &$h($n);
	    if ($prop) {
		print ",\n" if $need_comma;
		print $prop;
		$need_comma = 1;
	    }
	}

	attr($howto, $n,'att') if exists $$howto{'att'};
	attr($howto, $n,'xid') if exists $$howto{'xid'};

	if ($$howto{'chld'} && hasElementChildren($n)) {
	    my $chld_how = $$howto{'chld'};
	    my ($cname,$ctype) = @$chld_how;
	    print ",\n" if $need_comma; 
	    print "\"$cname\": $ctype\n";
	    $chld_closer = closer_of($ctype);
	    $need_comma = 0;
	}
	push @stack, "$chld_closer^$val_closer^$type_closer";

    } else {
	if ($howtos{'#auto'}) {
	    my $nam = $n->nodeName();
	    $nam =~ s/^.*?_//;
	    # FIXME: when the auto-node is a child that is part of an array which wraps
	    # a sequence of text() values, we need to auto-create the howto entry as:
	    #     $howto{'xmd_tr'} = { nam=>'#ignore', val=>'text()' };
	    #
	    $howtos{$n->nodeName()} = { 
		type=>'', nam=>$nam, val=>'text()', chld=>[$nam,'['], att=>'' 
	    };
	    goto AUTO_RETRY;
	} else {
	    warn "JSON.pm: no handler for element '",$n->nodeName,"'\n";
	}
    }

    return 1
	if ($$howto{'recurse'} && $$howto{'recurse'} eq 'no');
    return 0;
}

sub
node_end {
    my $n = shift @_;
    my $closer = pop @stack;
    if ($closer) {
	$closer =~ tr/^//d;
	print $closer, "\n";
	$need_comma = 1;
    }
}

##########################################################
my $nattr = 0;

sub
attr {
    my ($howto,$n,$prop) = @_;
    my @atts = ();
    my $att_how = '';
    $nattr = 0;
    if ($prop eq 'att') {
	@atts = $n->attributes();
	$att_how = $$howto{$prop};
    } else {
	my $p = $$howto{$prop};
	my ($ref,$how) = @$p;
	my $r = $n->getAttribute($ref);
	if ($r) {
	    my $idnode = $n->ownerDocument()->getElementById($r);
	    if ($idnode) {
		@atts = $idnode->attributes();
		$att_how = $how;
	    } else {
		return;
	    }		
	}
    }
    unless ($att_how eq '0') {
	if ($att_how eq '') {
	    jattrs($n,@atts);
	} else {
	    my $drop = 0;
	    if ($att_how =~ s/^-//) {
		$drop = 1;
	    }
	    my @a = split(/\s+/, $att_how);
	    my %a = (); @a{@a} = ();
	    
	    if ($drop) {
		my @natt = ();
		foreach my $a (@atts) {
		    unless (exists $a{$a->name()}) {
			push @natt, $a;
		    }
		}
		jattrs($n,@natt);
	    } else {
		my @natt = ();
		foreach my $a (@atts) {
		    if (exists $a{$a->name()}) {
			push @natt, $a;
		    }
		}
		jattrs($n,@natt);
	    }
	}
    }

    $need_comma = 1 if $nattr;
}

sub
jattrs {
    my $n = shift;
    my @att = @_;
    foreach my $a (@att) {
	next if $attmap{$a->name()} && $attmap{$a->name()} eq '#ignore';	    
	if ($need_comma) {
	    print ",\n";
	    $need_comma = 0;
	    ++$nattr;
	} else {
	    print ",\n" if $nattr++;
	}
	my $qname = $n->nodeName().':'.$a->name();
#	warn "jattrs: qname=$qname\n";
	if ($trigger_before{$qname}) {
	    my $h = $trigger_before{$qname};
	    my $prop = &$h($n);
	    if ($prop) {
		print ",\n" if $need_comma;
		print $prop;
		$need_comma = 1;
	    }
	}
	jattr($a);
	if ($trigger_after{$qname}) {
	    my $h = $trigger_after{$qname};
	    $need_comma = 1;
	    my $prop = &$h($n);
	    if ($prop) {
		print $prop;
		$need_comma = 1;
	    } elsif ($need_comma) {
#		print ",\n";
		$need_comma = 1;
	    }
	}
    }
}

sub
closer_of {
    if ($_[0] eq '{') { return '}' }
    elsif ($_[0] eq '[') { return ']' }
    else { return '' }
}

sub
default_metadata {
    my ($n,$pflag) = @_;
    my $p = '';
    if ($pflag) {
	$p = $n;
	chomp $p;
	$p =~ s/^.*?\":\s*\"(.*?)\".*$/$1/;
    } else {
	$p = $n->getOwnerDocument()->getDocumentElement()->getAttribute('project');
	unless ($p) {
	    $p = `oraccopt`;
	    chomp $p;
	}
    }
    print ",\n" if $need_comma; $need_comma = 0;
    my @props = ();
    push @props, "  \"project\": \"$p\"";
    push @props, "  \"source\": \"http://oracc.org/$p\"";
    push @props, "  \"license\": \"This data is released under the CC0 license\"";
    push @props, "  \"license-url\": \"https://creativecommons.org/publicdomain/zero/1.0/\"";
    push @props, "  \"more-info\": \"http://oracc.org/doc/opendata/\"";
    my $dt = DateTime->now;
    my $ds = $dt->iso8601; # . $dt->time_zone();
    push @props, "  \"UTC-timestamp\": \"$ds\"";
    return join(",\n", @props);
}

sub
jattr {
    my $a = shift;
    jprint($attmap{$a->name} || $a->name);
    print ": ";
    jprint($a->value);
    return 1;
}

sub
jprint {
    print '"',jsonify($_[0]),'"';
}

sub
jsonify {
    my $tmp = shift;
    $tmp =~ s/"/\000"/g;
    $tmp =~ s/\\/\000\\/g;
    $tmp =~ tr/\000/\\/;
    $tmp =~ tr/\x80-\x9f//d;
    $tmp =~ tr/\t\xa0/  /;
    $tmp;
}

sub
reset {
    $need_comma = 0;
}

### REUSABLE NAMESPACE HOWTOS

sub gdl_howtos {
    my($howto,$attmap) = @_;
    
   my $gdlattr = '-gdl_status gdl_accented';
# '-form gdl_accented gdl_queried gdl_break gdl_breakEnd gdl_breakStart gdl_status gdl_o gdl_c gdl_ho gdl_hc gdl_statusStart gdl_statusEnd';

    $$howto{'gdl_a'} = { type=>'{' , nam=>'a', val=>'text()', att=>$gdlattr };
    $$howto{'gdl_b'} = { type=>'{' , nam=>'b', val=>'text()', att=>$gdlattr };
    $$howto{'gdl_c'} = { type=>'{',  nam=>'c', val=>'@form', chld=>['seq','['], att=>$gdlattr.' form' };
    $$howto{'gdl_d'} = { type=>'{' , nam=>'det', val=>'@gdl_role', att=>'-gdl_role', chld=>['seq','[']};
    $$howto{'gdl_f'} = { type=>'{' , nam=>'f', val=>'text()', att=>$gdlattr };
    $$howto{'gdl_g'} = { type=>'{' , nam=>'g', val=>'text()', att=>$gdlattr, chld=>['seq','[']};
    $$howto{'gdl_m'} = { type=>'{' , nam=>'m', val=>'text()', att=>$gdlattr };
    $$howto{'gdl_n'} = { type=>'{' , nam=>'n', val=>'n', chld=>['seq','['], att=>$gdlattr };
    $$howto{'gdl_o'} = { type=>'{' , nam=>'o', val=>'@gdl_type', att=>$gdlattr.' gdl_type' };
    $$howto{'gdl_p'} = { type=>'{',  nam=>'p', val=>'@gdl_type', chld=>['punct','['], att=>$gdlattr.' gdl_type' };
    $$howto{'gdl_q'} = { type=>'{',  nam=>'q', val=>'@form', chld=>['qualified','['], att=>$gdlattr.' form' };
    $$howto{'gdl_r'} = { type=>'{' , nam=>'r', val=>'text()', chld=>['mods','['], att=>$gdlattr };
    $$howto{'gdl_s'} = { type=>'{' , nam=>'s', val=>'text()', chld=>['mods','['], att=>$gdlattr };
    $$howto{'gdl_v'} = { type=>'{' , nam=>'v', val=>'text()', chld=>['mods','['], att=>$gdlattr };
    $$howto{'gdl_w'} = {
	type=>'' , nam=>'#ignore', val=>'#ignore', att=>'-id',chld=>['gdl','['],
	formchild=>{ type=>'{' , nam=>'w', val=>'text()', chld=>['gdl','['], att=>$gdlattr,
		     object_force_nam=>'yes'
	}
    };
    $$howto{'gdl_x'} = { type=>'{' , nam=>'x', val=>'@gdl_type', att=>$gdlattr.' gdl_type' };
    $$howto{'gdl_gg'} = { type=>'{', nam=>'gg',val=>'@gdl_type', chld=>['group','['], att=>$gdlattr };
    $$howto{'gdl_nonw'} = { type=>'' , nam=>'#ignore', val=>'#ignore', att=>'-id',chld=>['gdl','['] };
    $$howto{'gdl_surro'} = { type=>'{', nam=>'surro',val=>'gg', chld=>['group','['], att=>$gdlattr };

    $$howto{'gdl_gloss'} = { type=>'{', nam=>'gloss',val=>'@gdl_type', att=>$gdlattr.' gdl_type',
			    chld=>['seq','[']
    };

    $$howto{'xtf_surro'} = { type=>'{', nam=>'surro',val=>'gg', chld=>['group','{'], att=>$gdlattr };

    $$howto{'norm_w'} = { type=>'#ignore', recurse=>'no' };
    $$howto{'norm_s'} = { type=>'#ignore' };

    $$attmap{'gdl_delim'} = 'delim';
    $$attmap{'gdl_logolang'} = 'logolang';
    $$attmap{'gdl_pos'} = 'pos';
    $$attmap{'gdl_role'} = 'role';
    $$attmap{'gdl_queried'} = 'queried';
    $$attmap{'gdl_break'} = 'break';
    $$attmap{'gdl_breakEnd'} = 'breakEnd';
    $$attmap{'gdl_breakStart'} = 'breakStart';
    $$attmap{'gdl_o'} = 'o';
    $$attmap{'gdl_c'} = 'o';
    $$attmap{'gdl_ho'} = 'ho';
    $$attmap{'gdl_hc'} = 'hc';
    $$attmap{'gdl_statusStart'} = 'statusStart';
    $$attmap{'gdl_statusEnd'} = 'statusStart';
}

1;


1;
