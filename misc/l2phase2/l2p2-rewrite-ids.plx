#!/usr/bin/perl
use warnings; use strict;
use open 'utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::NS;
binmode STDOUT, ':raw';

my $CBD = 'http://oracc.org/ns/cbd/1.0';
my $lang = shift @ARGV;
my %new_ids = ();

# This test is a bit edgy; when there is no corpus, we have no mapping to do
# but testing for a .map file is really not the way to determine that.
exit 0 unless -s "01bld/$lang/$lang.map";

warn "$0: rewriting IDs for glossary 01bld/$lang/$lang.xml\n";

system 'cp', "01bld/$lang/$lang.map", "01tmp/$lang-pre-rewrite-ids.map";
system 'cp', "01bld/$lang/$lang.xml", "01tmp/$lang-pre-rewrite-ids.xml";

load_ids(); # use Data::Dumper; open(D,'>D'); print D Dumper \%new_ids; close D;

map_ids_in_xml("01bld/$lang/$lang.xml");
map_ids_in_map("01bld/$lang/$lang.map");

# my @LID = eval("<01bld/$lang/L*.lst>");
# foreach my $lid (@LID) {
#     map_ids_in_lst($lid);
# }

################################################################
my $id_base = '';
my $new_id = 0;
my $oid = '';

sub
load_ids {
    my @ids = `xsltproc $ENV{'ORACC'}/lib/scripts/l2p2-list-ids.xsl 01bld/$lang/$lang.xml`;
    if ($#ids >= 0) {
	chomp @ids;
	foreach (@ids) {
	    $id_base = $oid = '' if s/^-//;
	    if (/^[ox]\d+$/) { # it's an OID
		$oid = $_;
		$new_id = 0;
		$new_ids{$_} = $_;
	    } else {
		if ($oid) {
		    $new_ids{$_} = sprintf("$oid.%d",$new_id++)
			unless defined $new_ids{$_};
		} elsif ($id_base) {
		    $new_ids{$_} = sprintf("$id_base.%d",$new_id++)
			unless defined $new_ids{$_};
		} else {
		    $id_base = $_;
		    $id_base =~ s/^.*?\.//;
		    $id_base =~ s/\..*$//;
		    $new_id = 0;
		    $new_ids{$_} = sprintf("$id_base%d",$new_id++)
			unless defined $new_ids{$_};
		}
	    }
	}
    }
}

sub
map_ids {
    my $n = shift;
    my $xid = xid($n);
    my $cid = $n->getAttributeNS($CBD,'id');
    my $ref = $n->getAttribute('ref');
    my $eref = $n->getAttribute('eref');
    my $morph2 = $n->getAttribute('morph2');
    if ($xid) {
	my $new_id = $new_ids{$xid};
	$n->setAttributeNS($XML,'id',$new_id) if $new_id;
    }
    if ($cid) {
	my $new_id = $new_ids{$cid} || '';
#	warn "found cid=$cid => $new_id\n";
	$n->setAttributeNS($CBD,'id',$new_id) if $new_id;
    }
    if ($ref) {
	if ($ref !~ /^\#/) { # FIXME: xrefs to external glo
	    my $new_ref = new_idrefs($ref);
	    $n->setAttribute('ref',$new_ref) if $new_ref;
	}
    }
    if ($eref) {
	my $new_ref = new_idrefs($eref);
	$n->setAttribute('eref',$new_ref) if $new_ref;
    }
    if ($morph2) {
	my $new_ref = new_idrefs($morph2);
	$n->setAttribute('morph2',$new_ref) if $new_ref;
    }
    foreach my $c ($n->childNodes()) {
	map_ids($c) if $c->isa('XML::LibXML::Element');
    }
}

sub
map_ids_in_lst {
    open(L,"$_[0]") || die;
    my @lid = (<L>);
    close(L);
    my @nid = ();
    foreach (@lid) {
	chomp;
	push @nid, $new_ids{$_};
    }
    open(L,">$_[0]");
    foreach my $n (sort grep(defined, @nid)) {
	print L "$n\n";
    }
    close(L);
}

sub
map_ids_in_map {
    open(M,"$_[0]") || die;
    my @map = (<M>);
    close(M);
    open(M,">$_[0]");
    foreach (@map) {
	/\s(\S+)$/;
	my $new_id = $new_ids{$1};
#	warn "mapping $1 to $new_id\n" if $new_id;
	s/(\s)\S+$/$1$new_id/ if $new_id;
	print M;
    }
    close(M);
}

sub
map_ids_in_xml {
    my $f = shift;
    my $x = load_xml($f) || die "failed to load XML from $f\n";
    map_ids($x->getDocumentElement());
    open(F,">$f"); binmode F;
    print F $x->toString();
    close(F);
}

sub
new_idrefs {
    my $old = shift;
    my @old = split(/\s+/, $old);
    my @new = ();
    foreach (@old) {
	if ($new_ids{$_}) {
	    push @new, $new_ids{$_};
	} elsif (/o\d+/) {
	    push @new, $_; # this allows ePSD2 to work; may need more tweaks for other projects
	} else {
	    warn "l2p2-rewrite-ids.plx: no new_id for old ID $_\n";
	}
    }
    join(' ', @new);
}

1;
