#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;

my $cbd = shift @ARGV;
my $iterate = 0;
my $xcbd = load_xml($cbd);

print "{\n";
iterate($xcbd->getDocumentElement());
print "}\n";

###########################################################

sub
iterate {
    my $n = shift;
    my $hasChildNodes = node_start($n);
    if ($iterate) {
        foreach my $c ($n->childNodes()) {
            my $isa = ref($c);
            if ($isa eq 'XML::LibXML::Element') {
                iterate($c);
            } elsif ($isa eq 'XML::LibXML::Text') {
                escape($c->data());
            } else {
                # skip PIs
            }
        }
    } else {
        $iterate = 1;
    }
    node_end($n);
}

sub
node_start {
    my $n = shift;
    print '"', $n->nodeName(), "\": ";
    my $f = $n->firstChild();
    if (ref($f) eq 'XML::LibXML::Element') {
	return 1;
    } else {
	return 0;
    }
}

sub
node_end {
    my $n = shift;
}

1;
