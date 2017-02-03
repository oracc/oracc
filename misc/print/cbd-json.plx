#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;

my $cbd = shift @ARGV;
my @stack = ();
my $depth = 0;
my $iterate = 0;
my $need_comma = 0;
my $xcbd = load_xml($cbd);

my %howto = ();

glossary_howtos();

###########################################################

iterate($xcbd->getDocumentElement());

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
		tabs();
		print ",\n";
		$need_comma = 0;
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
		    $nam = $attr->value();
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
	$need_comma = $nattr;

	if ($$howto{'chld'}) {
	    my $chld_how = $$howto{'chld'};
	    my ($cname,$ctype) = @$chld_how;
	    print ",\n" if $need_comma; $need_comma = 0;
	    tabs();
	    print "\"$cname\": $ctype\n";
	    $chld_closer = closer_of($ctype);
	}

#		push @stack, "(v)$val_closer(c)$chld_closer(t)$type_closer";
	push @stack, "$val_closer$chld_closer$type_closer";
	
    } else {
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
    $howto{'cbd_entries'} = {
	type=>"{",
	nam=>'type',val=>'@n',
	att=>'project xml_lang',
	chld=>[ 'entries','[' ]
    };
    $howto{'cbd_entry'} = {
	type=>"{",
	nam=>'headword',val=>'@n',
	att=>'-n'
    };
    $howto{'cbd_cf'} = { nam=>'cf',val=>'text()' };
    $howto{'cbd_gw'} = { nam=>'gw',val=>'text()' };
    $howto{'cbd_pos'} = { nam=>'pos',val=>'text()' };
    $howto{'cbd_mng'} = { nam=>'mng',val=>'text()' };
    $howto{'cbd_forms'} = { nam=>'forms',val=>'[' };
    $howto{'cbd_form'} = { type=>"{",nam=>'type',val=>'form',att=>'' };
    $howto{'cbd_t'} = { type=>'#ignore' };
    $howto{'cbd_norms'} = { nam=>'norms',val=>'[' };
    $howto{'cbd_norm'} = { type=>"{",nam=>'#ignore',val=>'#ignore',att=>'' };
    $howto{'cbd_n'} = { nam=>'n',val=>'text()',att=>'' };
    $howto{'cbd_f'} = { type=>"{",nam=>'type',val=>'normform',att=>'' };
    $howto{'cbd_senses'} = {
	nam=>'senses',val=>'[',
    };
    $howto{'cbd_sense'} = {
	type=>"{",
	nam=>'type',val=>'sense',
	att=>''
    };
    $howto{'cbd_sigs'} = { nam=>'sigs',val=>'[' };
    $howto{'cbd_sig'} = { type=>"{",nam=>'type',val=>'sig',att=>'' };
}
1;
