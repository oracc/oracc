#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
use lib '/usr/local/share/cdl/tools';
use lib '/usr/local/share/cdl/signlists/psl/tools';
use CDL::SE::Indexer;
use CDL::SE::XML;
use CDL::SE::DBM;
use Aliases;
use XML::LibXML; my $xp = XML::LibXML->new();
my $ixname = 'epsd-se';
my $ix = CDL::SE::Indexer::index($ixname,'ePSD CBD Index','x1',1,
				 ['X','c','t','s','a','e'],
				 ['X','c','t','s','a'],
				 0, 5000);

CDL::SE::Indexer::fuzzy($ix,'t',1,Aliases::as_array());

my %summary = ();
my $xhtml_uri = 'http://www.w3.org/1999/xhtml';
my $sdoc = $xp->parse_file('results/summaries.html');
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
#open(IN,"tools/getkeys.sh|");
#while (<IN>) {
##    next if $subset++ % 20;
#    if (/^(..):(e\d+):(.*)$/) {
#	my($fld,$id,$key) = ($1,$2,$3);
#	push @{$rec{$id}}, [ $fld,$key ];
#    } else {
#	die "syntax error: $_";
#    }
#}
#close(IN);

my $id;
open(IN,"xsltproc tools/se-keys.xsl results/articles.cbd|");
while (<IN>) {
#    next if $subset++ % 1000;
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

my $cbdx = 'results/articles.cbd';
my $cbd = $xp->parse_file($cbdx);

foreach my $r (sort { &ecmp } keys %rec) {
    my $ixid = CDL::SE::Indexer::record($ix,undef,$r,'',$summary{$r});
    my %units = ();
    foreach my $k (@{$rec{$r}}) {
	my($fld,$key) = @$k;
#	$fld =~ s/.$//;
	$key =~ tr/\/()|?!:;=\#[]/             /;
	$key =~ tr/&/^/;
	$key =~ tr/,\`\'\"~*//d;
	$key =~ s/\s+/ /g;
	my @keys = grep($_,split(/[- \.{}]+/, $key));

	my $fuid = CDL::SE::Indexer::field_unit($ix,$fld,$ixid);
	CDL::SE::Indexer::field_grep($ix,$fld,$key,$fuid);
	foreach my $i (@keys) {
	    CDL::SE::Indexer::item($ix,$fuid,undef,$i,undef,$fld);
	}

	$fuid = CDL::SE::Indexer::field_unit($ix,'X',$ixid);
	CDL::SE::Indexer::field_grep($ix,'X',$key,$fuid);
	foreach my $i (@keys) {
	    CDL::SE::Indexer::item($ix,$fuid,undef,$i,undef,'X');
	}
    }
}
close(IN);

CDL::SE::XML::toXML($ix);
CDL::SE::DBM::create_ascii($ixname);

sub
ecmp {
    my $e1 = $a;
    my $e2 = $b;
    $e1 =~ s/^e//;
    $e2 =~ s/^e//;
    $e1 <=> $e2;
}

1;
