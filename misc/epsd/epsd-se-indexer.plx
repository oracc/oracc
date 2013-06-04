#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
use lib '/usr/local/oracc/lib';
use ORACC::SE::Indexer;
use ORACC::SE::XML;
use ORACC::SE::DBM;
use ORACC::Aliases;
use XML::LibXML; my $xp = XML::LibXML->new();
my $ixname = 'epsd-se';
my $ix = ORACC::SE::Indexer::index($ixname,'ePSD CBD Index','x1',1,
				 ['X','c','t','s','a','e'],
				 ['X','c','t','s','a'],
				 0, 5000);

ORACC::SE::Indexer::fuzzy($ix,'t',1,ORACC::Aliases::as_array());

my %summary = ();
my $xhtml_uri = 'http://www.w3.org/1999/xhtml';
my $sdoc = $xp->parse_file('02www/cbd/sux/summaries.html');
foreach my $s ($sdoc->getDocumentElement()->getElementsByTagNameNS($xhtml_uri,
								   'p')) {
    my $key = $s->getAttribute('id');
    if ($key) {
	my $summary = ($s->childNodes())[0]->toString();
	$summary{$key} = $summary;
    }
}

my %rec = ();
my $subset = 0;

my $id;
open(IN,"xsltproc 00bin/se-keys.xsl 01bld/sux/articles.xml|");
while (<IN>) {
    if (/^id:(.*)$/) {
	$id = $1;
    } elsif (/^(.):(.*?)$/) {
	my($fld,$key) = ($1,$2);
	if ($fld eq 'a') {
	    $key =~ tr/āēīūâêîû/aeiuaeiu/;
	    $key =~ s/\(?m\)?$//;
	}
	if ($key) {
	    push @{$rec{$id}}, [ $fld,$key ];
	}
    } else {
	die "syntax error: $_";
    }
}
close(IN);

my $cbdx = '01bld/sux/articles.xml';
my $cbd = $xp->parse_file($cbdx);

foreach my $r (sort { &ecmp } keys %rec) {
    my $ixid = ORACC::SE::Indexer::record($ix,undef,$r,'',$summary{$r});
    my %units = ();
    foreach my $k (@{$rec{$r}}) {
	my($fld,$key) = @$k;
#	$fld =~ s/.$//;
	$key =~ tr/\/()|?!:;=\#[]/            /;
	$key =~ s/\\&/^/;
	$key =~ tr/&/^/;
	$key =~ tr/,\`\'\"~*//d;
	$key =~ s/\s+/ /g;
	my @keys = grep($_,split(/[- \.{}]+/, $key));

	my $fuid = ORACC::SE::Indexer::field_unit($ix,$fld,$ixid);
	ORACC::SE::Indexer::field_grep($ix,$fld,$key,$fuid);
	foreach my $i (@keys) {
	    ORACC::SE::Indexer::item($ix,$fuid,undef,$i,undef,$fld);
	}

	$fuid = ORACC::SE::Indexer::field_unit($ix,'X',$ixid);
	ORACC::SE::Indexer::field_grep($ix,'X',$key,$fuid);
	foreach my $i (@keys) {
	    ORACC::SE::Indexer::item($ix,$fuid,undef,$i,undef,'X');
	}
    }
}

ORACC::SE::XML::toXML($ix);
ORACC::SE::DBM::create($ixname);

#ORACC::SE::DBM::Dumper_out($ix,$ixname);

sub
ecmp {
    my $e1 = $a;
    my $e2 = $b;
    $e1 =~ s/^sux.x//;
    $e2 =~ s/^sux.x//;
    $e1 <=> $e2;
}

1;
