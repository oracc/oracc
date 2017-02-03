#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;

my @stack = ();
my $depth = 0;
my $iterate = 0;
my $need_comma = 0;

my %howto = ();

glossary_howtos();

my $projcbd = shift @ARGV;
my ($project,$lang) = split(/:/, $projcbd);

my $cbd_ns = "$ENV{'ORACC'}/bld/$project/$lang/$lang.g2x";
my $xis_ns = "$ENV{'ORACC'}/bld/$project/$lang/$lang.xis";

###########################################################

print "{\n";
print "\t\"type\": \"glossary\",\n";
print "\t\"project\": \"$project\",\n";
print "\t\"lang\": \"$lang\",\n";

my @in = `cat $cbd_ns | $ENV{'ORACC'}/bin/xns`;
my $cbd_nons = join('', @in);
my $xcbd = load_xml_string($cbd_nons);
iterate($xcbd->getDocumentElement());
$xcbd = undef;
$cbd_nons = undef;
print "\n,\n"; $need_comma = 0;
@in = `cat $xis_ns | $ENV{'ORACC'}/bin/xns`;
my $xis_nons = join('',@in);
my $xxis = load_xml_string($xis_nons);
iterate($xxis->getDocumentElement());
$xxis = undef;

print "\n}\n";

###########################################################

sub
iterate {
    my $n = shift;
    my $ecount = 0;
    
    node_start($n);

    foreach my $c ($n->childNodes()) {
	my $isa = ref($c);
	# the input will be Oracc data, no whitespace, no mixed content
	if ($isa eq 'XML::LibXML::Element') {
	    if ($ecount++) {
#		tabs();
#		print ",\n";
		$need_comma = 1;
	    }
    
#	    ++$depth;
	    iterate($c);
#	    --$depth;
	}
    }
    node_end($n);
}

sub
node_start {
    my $n = shift;
    my $nm = $n->nodeName();
    my $howto = $howto{$nm};

    my $type_closer = '';
    my $val_closer = '';
    my $chld_closer = '';
    
    if ($howto) {

	my $type_how = $$howto{'type'} || '';

	if ($type_how eq '#ignore') {
	    push @stack, '';
	    return;
	}
	
	if ($need_comma) {
	    #	    print "<nc>,\n";
	    print ",\n";
	    $need_comma = 0;
	}

	if ($type_how) {
	    $type_closer = closer_of($type_how);
	    tabs();
	    print $type_how, "\n";
#	    ++$depth;
	}
	
	my $nam = $nm;
	my $nam_how = $$howto{'nam'};
	unless ($nam_how eq '#ignore') {
	    if ($nam_how =~ /^\@(\S+)/) {
		my $aname = $1;
		my $attr = undef;
		if ($attr = $n->getAttribute($aname)) {
		    $nam = $attr;
		}
	    } elsif ($nam_how eq 'text()') {
		$nam = $n->textContent();
	    } elsif (length $nam_how) {
		$nam = $nam_how;
	    } else {
		$nam = $nm;
	    }
	    tabs();
	    jprint($nam);
	    print ": ";
	}
	 
	my $val = undef;
	my $val_how = $$howto{'val'};
	unless ($val_how eq '#ignore') {
	    # Value must always push closer information 
	    # onto @stack unless $val_how = #ignore
	    if ($val_how =~ /^\@(\S+)/) {
		my $aname = $1;
		my $attr = undef;
		if ($attr = $n->getAttribute($aname)) {
		    jprint($attr);
		    print ",\n";
		} else {
		    warn "element $nm wants value from absent attribute $aname\n";
		}
		$val_closer = '';
	    } elsif ($val_how =~ /^[\[\{]$/) {
		print $val_how;
		$val_closer = closer_of($val_how);
		if ($$howto{'text'}) {
		    print "\n";
		    tabs();
		    jprint($$howto{'text'});
		    print ": ";
		    jprint($n->textContent());
		    $need_comma = 1;
		}
	    } elsif ($val_how =~ /text()/) {
		jprint($n->textContent());
#		print ",\n";
		$need_comma = 1;
		$val_closer = '';
	    } elsif (length $val_how) {
		$val = $val_how;
		jprint($val_how);
#		print ",\n";
		$need_comma = 1;
		$val_closer = '';
	    } else {
		$val = '{';
		$val_closer = '}';
	    }
	}

	my $nattr = 0;
	if (exists $$howto{'att'}) {
	    my $att_how = $$howto{'att'};
	    unless ($att_how eq '0') {
		if ($att_how eq '') {
		    foreach my $a ($n->attributes) {
			if ($need_comma) {
			    #			    print "<166>,\n";
			    print ",\n";
			    $need_comma = 0;
			}
			print ",\n" if $nattr++;
			jattr($a);
		    }
		} else {
		    my $drop = 0;
		    if ($att_how =~ s/^-//) {
			$drop = 1;
		    }
		    my @a = split(/\s+/, $att_how);
		    my %a = (); @a{@a} = ();

		    if ($drop) {
			foreach my $a ($n->attributes()) {
			    unless (exists $a{$a->name()}) {
				if ($need_comma) {
				    print ",\n";
				    $need_comma = 0;
				}
				print ",\n" if $nattr++;
				jattr($a);
			    }
			}
		    } else {
			foreach my $a ($n->attributes()) {
			    if (exists $a{$a->name()}) {
				if ($need_comma) {
				    print ",\n";
				    $need_comma = 0;
				}			
				print ",\n" if $nattr++;
				jattr($a);
			    }
			}
		    }
		}
	    }
	}
	$need_comma = 1 if $nattr;
	
	if ($$howto{'chld'}) {
	    my $chld_how = $$howto{'chld'};
	    my ($cname,$ctype) = @$chld_how;
	    #	    print "<212>,\n" if $need_comma; $need_comma = 0;
	    print ",\n" if $need_comma; $need_comma = 0;
	    tabs();
	    print "\"$cname\": $ctype\n";
	    $chld_closer = closer_of($ctype);
	}

#		push @stack, "(c)$chld_closer(v)$val_closer(t)$type_closer";
	push @stack, "$chld_closer$val_closer$type_closer";
	
    } else {
	warn "cbd-json.plx: no handler for element '",$n->nodeName,"'\n";
#	tabs();
#	jprint($nm);
#	print ": {\n";
#	push @stack, '}';
    }
}

sub
node_end {
    my $n = shift @_;
    my $closer = pop @stack;
    if ($closer) {
	tabs();
	#		print '<',$n->nodeName(),'>',$closer, "\n";
	print $closer, "\n";
    }
}

##########################################################

sub
closer_of {
    if ($_[0] eq '{') { return '}' }
    elsif ($_[0] eq '[') { return ']' }
    else { return '' }
}

sub
jattr {
    my $a = shift;
    tabs();
    jprint($a->name);
    print ": ";
    jprint($a->value);
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
tabs {
    print "\t"x$depth;
}

##############################################

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
#   ?? 1 -- attributes are wrapped in "props" hash
#
sub
glossary_howtos {
#    $howto{'cbd_entries'} = {
#	type=>"{",
#	nam=>'type',val=>'@n',
#	att=>'project xml_lang',
#	chld=>[ 'entries','[' ]
    #   };
    $howto{'cbd_entries'} = {
	nam=>"entries", val=>'[',
    };
    $howto{'cbd_entry'} = {
	type=>"{",
	nam=>'headword',val=>'@n',
	att=>'-n'
    };
    $howto{'cbd_cf'} = { nam=>'cf',val=>'text()' };
    $howto{'cbd_gw'} = { nam=>'gw',val=>'text()' };
    $howto{'cbd_pos'} = { nam=>'pos',val=>'text()' };
    $howto{'cbd_epos'} = { nam=>'epos',val=>'text()' };
    $howto{'cbd_mng'} = { nam=>'mng',val=>'text()' };
    $howto{'cbd_forms'} = { nam=>'forms',val=>'[' };
    $howto{'cbd_form'} = { type=>"{",nam=>'type',val=>'form',att=>'' };
    $howto{'cbd_t'} = { type=>'#ignore' };
    $howto{'cbd_cof-form-norm'} = { nam=>'cof-form-norm',val=>'text()',att=>'' };
    $howto{'cbd_norms'} = { nam=>'norms',val=>'[' };
    $howto{'cbd_norm'} = { type=>"{",nam=>'#ignore',val=>'#ignore',att=>'' };
    $howto{'cbd_n'} = { nam=>'n',val=>'text()',att=>'' };
    $howto{'cbd_f'} = { type=>"{",nam=>'type',val=>'normform',att=>'' };
    $howto{'cbd_bases'} = { nam=>'bases',val=>'[' };
    $howto{'cbd_base'} = { type=>"{",nam=>'type',val=>'base',att=>'' };
    $howto{'cbd_morphs'} = { nam=>'morphs',val=>'[' };
    $howto{'cbd_morph'} = { type=>"{",nam=>'type',val=>'morph',att=>'' };
    $howto{'cbd_conts'} = { nam=>'conts',val=>'[' };
    $howto{'cbd_cont'} = { type=>"{",nam=>'type',val=>'cont',att=>'' };
    $howto{'cbd_compound'} = { nam=>'compound',val=>'[' };
    $howto{'cbd_cpd'} = { type=>"{",nam=>'type',val=>'cpd',att=>'' };
    $howto{'cbd_senses'} = { nam=>'senses',val=>'[' };
    $howto{'cbd_sense'} = { type=>"{",nam=>'type',val=>'sense',att=>'' };
    $howto{'cbd_sigs'} = { nam=>'sigs',val=>'[' };
    $howto{'cbd_sig'} = { type=>"{",nam=>'type',val=>'sig',att=>'' };
    $howto{'cbd_cof-data'} = { nam=>'cof-data',val=>'{',att=>'' };
    $howto{'cbd_cof-head'} = { nam=>'head',val=>'text()',att=>'0' };
    $howto{'cbd_cof-tail'} = { nam=>'tail',val=>'{',att=>'',text=>'sig' };

    $howto{'xis_xisses'} = {
	nam=>'instances',
	val=>'{'
    };
    $howto{'xis_xis'} = {
	nam=>'@xml:id',
	val=>"[",
	att=>'0',
    };
    $howto{'xis_r'} = { nam=>'#ignore', val=>'text()' };
    $howto{'xis_periods'} = { type=>'#ignore' };
}
1;
