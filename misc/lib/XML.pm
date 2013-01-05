package ORACC::XML;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/childText firstChildElement load_xml load_xml_chunk 
             load_xml_string load_xsl 
	     parse_xsl rnv tags xmldecl xmlify xid/;

use warnings; use strict;
use XML::LibXML;
my $xp = XML::LibXML->new(); $xp->validation(0); $xp->expand_xinclude(1);
use XML::LibXSLT; my $xsl = XML::LibXSLT->new();
#use Data::Dumper;

sub
childText {
    my($n,$ns,$tag) = @_;
    my $c = (tags($n,$ns,$tag))[0];
    return '' unless $c;
    $c->textContent();
}

sub
firstChildElement {
    my $node = shift;
    if ($node->isa('XML::LibXML::Document')) {
	$node->getDocumentElement();
    } else {
	my $c = undef;
	foreach my $child ($node->childNodes()) {
	    if ($child->isa('XML::LibXML::Element')) {
		$c = $child;
		last ;
	    }
	}
	$c;
    }
}

sub
load_xml {
    my($xml_file,$enotfound) = @_;
    if ($xml_file eq '-') {
	my $x = '';
	local($/) = undef;
	$x = <>;
	load_xml_string($x);
    } elsif (-r $xml_file) {
	my $ret = undef;
	eval { $ret = $xp->parse_file($xml_file) };
	warn "ORACC::XML: $@\n" if $@;
	$ret;
    } else {
	warn("ORACC::XML: XML file $xml_file not readable\n")
	    if $enotfound;
	undef;
    }
}

sub
load_xml_chunk {
    my $xml_str = shift;
    if ($xml_str) {
	my $ret = undef;
	eval { $ret = $xp->parse_balanced_chunk($xml_str) };
	warn "ORACC::XML: $@\n" if $@;
	$ret;
    } else {
	warn("ORACC::XML: load_xml_chunk called on empty input");
	undef;
    }
}

sub
load_xml_string {
    my $xml_str = shift;
    if ($xml_str) {
	my $ret = undef;
	eval { $ret = $xp->parse_string($xml_str) };
	warn "ORACC::XML: $@\n" if $@;
	$ret;
    } else {
	warn("ORACC::XML: load_xml_string called on empty input");
	undef;
    }
}

sub
load_xsl {
    my ($xsl_file) = @_;
    if (-r $xsl_file) {
	my $sheet = load_xml($xsl_file);
	$xsl->parse_stylesheet($sheet);
    } else {
	warn("ORACC::XML: XSL argument file $xsl_file not readable\n");
	undef;
    }
}

sub
parse_xsl {
    my $sheet = shift;
    $xsl->parse_stylesheet($sheet);
}

sub
rnv {
    my ($rnc,$doc,$verbose) = @_;
    $doc = '' unless $doc;
    if ($verbose) {
	if ($doc) {
	    print STDERR "validating $doc ...\n";
	} else {
	    print STDERR "checking grammar $rnc ...\n";
	}
    }
    system("@@ORACC@@/bin/rnv -q -c $rnc $doc 2>rnv.log");
    unless (-z 'rnv.log') {
	# seems like there should be an easier way to cat
	# to stderr than the following...
	exec 'perl', '-ne', 'print STDERR', 'rnv.log';
    } else {
	unlink 'rnv.log';
    }
}

sub
tags {
    my($node,$ns,$tag) = @_;
    $node = $node->getDocumentElement()
	if $node->isa('XML::LibXML::Document');
    if ($ns) {
	$node->getElementsByTagNameNS($ns,$tag);
    } else {
	$node->getElementsByTagName($tag);
    }
}


# Convenience function to transform a flat list of LibXML nodes into
# a hierarchical structure.
sub
tree {
    my @nodes = @_;
    my @ret = ();
    my @leftovers = ();
    my $top = undef;
#    while ($#nodes >= 0 && $nodes[0] && $nodes[0]->hasAttribute('level')) {
    while ($#nodes >= 0 && defined($nodes[0]) && $nodes[0]->hasAttribute('level')) {
	($top,@leftovers) = _subtree(-1,@nodes);
	if ($top->getAttribute('type') eq 'clear') {
	    push @ret, $top->childNodes();
	} else {
	    push @ret, $top;
	}
	last unless $#leftovers >= 0;
	@nodes = @leftovers;
    }
    if ($#ret < 0) {
	return @nodes;
    } else {
	push @ret, @leftovers if $#leftovers >= 0;
	return @ret;
    }
}

sub
_subtree {
    my ($parent_level,@n) = @_;
    my $new_parent = shift @n;
    my $new_parent_level = $new_parent->getAttribute('level');
    while ($#n >= 0) {
	if ($n[0]->isa('XML::LibXML::Element') 
	    && $n[0]->hasAttribute('level')) {
	    my $next_level = $n[0]->getAttribute('level');
	    if ($next_level > $new_parent_level) {
		my $top;
		($top,@n) = _subtree($new_parent_level,@n);
		$new_parent->appendChild($top);
	    } else {
		return ($new_parent,@n);
	    }
	} else {
	    $new_parent->appendChild(shift @n);
	}
    }
    return ($new_parent,@n);
}

sub
xmldecl {
    '<?xml version="1.0" encoding="utf-8"?>'."\n";
}

sub
xmlify {
    return '' unless $_[0];
    my $tmp = shift;
    if ($tmp && $tmp =~ /[&<\"]/) {
	$tmp =~ s/&/&amp;/g;
	$tmp =~ s/</&lt;/g;
	$tmp =~ s/\"/&quot;/g
    }
    $tmp =~ tr/\000-\cH\cK\cL\cO-\037//d;
    $tmp;
}

sub
xid {
    my $node = shift;
    return '' unless $node;
    $node = $node->getDocumentElement()
	if $node->isa('XML::LibXML::Document');
    return $node->getAttributeNS('http://www.w3.org/XML/1998/namespace','id')
	|| '';
}

1;
