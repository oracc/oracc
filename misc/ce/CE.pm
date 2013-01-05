package ORACC::CE;
use lib "$ENV{'ORACC'}/lib";
use warnings; use strict;
use ORACC::Expand2;
use ORACC::XML;

my $kwic_window = 3;
my $min_prev_rows = 3;
my %loaded = ();
my $xml_uri = 'http://www.w3.org/XML/1998/namespace';

sub
load {
}

sub
unload {
}

sub
kwic {
    die "CDL:CE: kwic context not yet implemented\n";
}

sub
line {
    my($id,$select,$project) = @_;
    my @ret = ();
    my $cid = $id;
    if ($cid =~ /^Q.*?\.c.*?/) {
	$cid =~ s/\.w.*$//;
    } else {
	$cid =~ s/\.(.*?)\.[^.]+$/.$1/;
    }
    my $lid = $cid;
    my $pq = $cid;
    $pq =~ s/\..*$//;
    my $txh = $loaded{$pq};
    unless ($txh) {
	if ($project) {
	    my $txhfname = expand_in_project(undef,"$pq.txh", $project);
	    if (($txh = load_xml($txhfname))) {
#		warn "CE.pm: $txhfname ok\n";
	    } else {
		warn "CE.pm: $txhfname failed\n";
	    }
	    $loaded{$pq} = $txh;
	}
	$txh = $loaded{$pq} || load_xml(expand_no_project("$pq.txh"));
	return "<label></label><p>(NO CONTEXT FOR $cid)</p>" unless $txh;
	$loaded{$pq} = $txh unless $loaded{$pq};
    }
    my $c = $txh->getElementsById($cid);
    return "<label></label><p>(NO LINE FOR $cid)</p>" unless $c;
    my $cid_node = $c;
    for (my $i = $min_prev_rows; $i; ) {
	$cid_node = $cid_node->previousSibling();
	if ($cid_node) {
	    if ($cid_node->isa('XML::LibXML::Element')
		&& $cid_node->localName() eq 'tr') {
		--$i;
		last unless $i;
	    }
	} else {
	    last;
	}
    }
    if ($cid_node && $cid_node->isa('XML::LibXML::Element')) {
	$cid = $cid_node->getAttributeNS($xml_uri,'id') || '';
    } else {
	$cid = '';
    }
    my $lp = $c->toString();
    $lp =~ s,^<span,<span,;
    $lp =~ s,</tr>,,;
    $lp =~ m,<span class="xlabel">(.*?)</span>,;
    my $label = $1;
    my $line = $lp;
    $line =~ s#^.*?</td>##s;
#    $line =~ s/xml:id/id/g;
    if ($label && $line) {
	$line =~ s/class=\"w\"(?= id=\"$id\")/class=\"selected\"/
	    || $line=~ s/class=\"w\"(?<=id=\"$id\" )/class=\"selected\"/;
	
	my $txh_table_node = $txh->getDocumentElement();
	unless ($txh_table_node eq 'table') {
	    foreach my $x ($txh_table_node->childNodes()) {
		next unless $x->isa('XML::LibXML::Element');
		$txh_table_node = $x;
		last;
	    }
	}
	my $label_base = $txh_table_node->getAttribute('xlabel');
	
	$label_base =~ s/\s+\[\]$//;
	$label = "($label_base $label)";
	($label,"<p>$line</p>",$lid,$cid);
    } else {
	warn("CE: bad label/line in $lp\n");
	('bad','<p>bad</p>','','')
    }
}

1;
