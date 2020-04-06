#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
use XML::LibXML;

binmode STDERR, ':utf8'; binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

#my $xix_uri = 'http://oracc.org/xix/1.0';

my %sortmap = ();
my $sort_id = 0;
my %seen;

my $ix_file = shift @ARGV;
my $xp = XML::LibXML->new();
my $ix = $xp->parse_file($ix_file);
$ix->setEncoding('utf-8');
my $ix_root = $ix->getDocumentElement();

my $sort_file = (-d '01tmp' ? "01tmp/$$.sort" : "$$.sort");
open SORT, "|$ENV{'ORACC'}/bin/psdsort +1 +2 +3 -G >$sort_file";
foreach my $n ($ix_root->childNodes()) {
    my $p = printkey($n);
    print SORT $p if $p;
}
close SORT;

open IN, $sort_file;
my @results = (<IN>);
close(IN);
#unlink $sort_file;

$ix_root->removeChildNodes();
my $preamble = $ix_root->toString();
$preamble =~ s,/>$,>,;

open OUT, ">$ix_file.srt"; binmode(OUT, ':raw'); select OUT;
print '<?xml version="1.0" encoding="utf-8"?>',"\n";
print $preamble;
foreach my $r (@results) {
    $r =~ /^(\d+)/;
    print $sortmap{$1}->toString(0,1);
}
print '</index>';

1;

#########################################################

sub
printkey {
    my $n = shift;

    my $sk = $n->getAttribute('sortkey');
    my $sktmp = $sk;
    $sktmp =~ s/\{\+.*?\}//g;
    $sktmp =~ s/ₓ\(.*?\)/ₓₓ/g;
    $sktmp =~ tr/ʾ//d;
    if ($sk ne $sktmp) {
	$sk = $sktmp;
	$n->setAttribute('sortkey', $sk);
#	warn "reset sortkey $sk\n";
    }

    my $letterkey = $sk; $letterkey =~ s/\{.*?\}//g; $letterkey =~ tr/|()ʾ//d;
    $n->setAttribute('letterkey',$letterkey);
    
    my $ref = ($n->childNodes())[1]->getAttribute('ref');

    return undef if !$sk || $sk !~ /[a-zA-Z0-9]/ || ${$seen{$ref}}{$sk}++;

    ++$sort_id;
    $sortmap{$sort_id} = $n;
    
    my $sk0 = $sk;
    $sk0 =~ s/\{.*?\}//g;

    my $sk2 = 'a';
    $sk2 = 'x' if $n->firstChild()->lastChild()->hasChildNodes();

    $sk =~ tr/ /_/;
    $sk0 =~ tr/ /_/;
    $sk2 =~ tr/ /_/;

    $sk0 =~ s/^/~/ if $sk0 =~ /^[0-9]/;
    
    "$sort_id $sk0 $sk $sk2\n";
}
