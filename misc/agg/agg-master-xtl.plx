#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
my $linkbase = "$ENV{'ORACC'}/agg/master-linkbase.xml";

my %P = ();
my %PQ = ();
my %Q = ();
my %codes = ();
my %exemplar_lists = ();
my %exemplar_links = ();
my %parallel_lists = ();
my %source_lists = ();
my %sp = ();

create_rel_lists();
create_master_xtl();
#use Data::Dumper; print Dumper \%sp;

1;

############################################################################

sub
create_rel_lists {
    my $xlinkbase = load_xml($linkbase);

    foreach my $p ($xlinkbase->getDocumentElement()->childNodes()) {
	next unless $p->isa('XML::LibXML::Element');
	my $project = $p->getAttribute('n');
	foreach my $l ($p->childNodes()) {
	    next unless $l->isa('XML::LibXML::Element');
	    if ($l->localName() eq 'link') {
		my $rel = $l->getAttribute('rel');
		my($f,$t) = ($l->firstChild(),$l->lastChild());
		my $from_ref = $f->getAttribute('ref');
		my $to_ref   = $t->getAttribute('ref');
		my $from_line = $f->getAttribute('line');
		my $to_line   = $t->getAttribute('line');
		++$PQ{$from_ref};
		++$PQ{$to_ref};
		++$P{$from_ref};
		++$Q{$to_ref};
		if ($rel eq 'goesto') {
		    push @{$exemplar_lists{$to_line}}, $from_line;
		    push @{$exemplar_links{$from_ref}}, [ $from_line, $to_line ];
		} elsif ($rel eq 'parallels') {
		    push @{$parallel_lists{$f->getAttribute('line')}}, $t->getAttribute('line');
		    push @{$parallel_lists{$t->getAttribute('line')}}, $f->getAttribute('line');
		} elsif ($rel eq 'comesfrom') {
		    push @{$source_lists{$f->getAttribute('line')}}, $t->getAttribute('line');
		} else {
		    die; #can't happen unless the system is goofed up
		}
	    } elsif ($l->localName() eq 'refs') {
		my $t = $l->getAttribute('type');
		my $ref = $l->getAttribute('ref');
		if ($t eq 'sources') {
		    ${$sp{$ref}}[0] = [ $project, $l];
		} elsif ($t eq 'parallels') {
		    ${$sp{$ref}}[1] = [ $project, $l];
		} else {
		    die; #can't happen unless linkbase is bad
		}
	    } else {
		die; #can't happen unless linkbase is bad
	    }
	}
    }
}

sub
create_master_xtl {
    open(XTL,">$ENV{'ORACC'}/agg/master.xtl");
    print XTL xmldecl();
    print XTL "<list xmlns=\"http://oracc.org/ns/list\/1.0\">";
    foreach my $pq (sort keys %sp) {
	print XTL "<record xml:id=\"$pq\">";
	my($s,$p) = @{$sp{$pq}};
	if ($s) {
	    print XTL '<group type="Sources">';
	    sprefs(@$s);
	    print XTL '</group>';
	}
	if ($p) {
	    print XTL '<group type="Parallels">';
	    sprefs(@$p);
	    print XTL '</group>';
	}
	print XTL '</record>';
    }
    print XTL '</list>';
    close(XTL);
}

sub
sprefs {
    my ($project, $lnode) = @_;
    my @items = ();
    foreach my $c ($lnode->childNodes()) {
	my $r = "$project\:".$c->getAttribute('ref');
	push @items, [ $r, "<item text=\"$r\"/>" ];
    }
    print XTL map { $$_[1] } sort { cmp_by_pq($$a[0], $$b[0]) } @items;
}

sub
cmp_by_pq {
    my ($a_proj,$a_text) = ($_[0] =~ /^(.*?):(.*?)$/);
    my ($b_proj,$b_text) = ($_[1] =~ /^(.*?):(.*?)$/);
    if ($a_text eq $b_text) {
	return $a_proj cmp $b_proj;
    } else {
	return $a_text cmp $b_text;
    }
}

1;
