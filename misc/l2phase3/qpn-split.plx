#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::NS;
use Getopt::Long;

# $cbd is the name of the CBD to split, usually qpn.cbd
#
# $how is a comma-separated string giving POS's to group in
#   a single output. Multiple POS's in one output are 
#   joined by plus signs:
#   -how=RN+PN,DN,TN,ON,GN+SN
#
# $out is an output directory

my $cbd = '';
my $how = '';
my $out = '';

GetOptions(
    'cbd:s'=>\$cbd,
    'how:s'=>\$how,
    'out:s'=>\$out,
    );

my $xcbd = load_xml($cbd) || die "qpn-split.plx: can't load CBD $cbd\n";
my %outputs = ();
my %skip = ();

setup_outputs();
process_entries($xcbd->getDocumentElement()->childNodes());
dump_outputs();

#######################################################################

sub
dump_outputs {
    foreach my $o (keys %outputs) {
	next if $skip{$o};
	my $ofile = "$out/qpn-x-$o.cbd";
	open(O,">$ofile") || die "qpn-split.plx: can't write $ofile\n";
	print O $outputs{$o}->toString();
	close(O);
    }
}

sub
get_pos {
    $_[0]->findnodes('cbd:pos');
}

sub
process_entries {
    my @nodes = @_;
    foreach my $e (@nodes) {
	next unless $e->isa('XML::LibXML::Element');
	if ($e->localName() eq 'letter') {
	    process_entries($e->childNodes());
	} else {
	    my $pos = get_pos($e);
	    if ($outputs{$pos}) {
		$outputs{$pos}->getDocumentElement()->appendChild($e);
	    }
	}
    }
}

sub
setup_outputs {
    my @h = ();
    my $clone = $xcbd->cloneNode(1);
    $clone->getDocumentElement()->removeChildNodes();
    foreach my $h (split(/,/,$how)) {
	my $newx = $clone->cloneNode(1);
	my $i = 0;
	foreach my $pos (split(/\+/,$h)) {
	    $outputs{$pos} = $newx;
	    ++$skip{$pos} if $i++;
	}
    }
}

1;
