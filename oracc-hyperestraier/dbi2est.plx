#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
my $project = `oraccopt`;
my @in = <02pub/*/keys.est>;
push @in, <02pub/cbd/*/keys.est>;

# assume for now that the URI to search any given index is:
#
# /[PROJ]/search/[INDEX]?TERMS
#

my %longcat = (
    cat=>'catalogue',
    txt=>'transliterations',
    tra=>'translations',
    lem=>'lemmatization'
    );

foreach my $in (@in) {
    my $idx = $in;
    my $cat = $idx;
    if ($cat =~ /cbd/) {
	$cat =~ 'cbd';
    } else {
	$cat =~ s#^02pub/(.*?)\.*$#$1#;
    }
    $idx =~ s#^02pub(?:/cbd)?/##;
    $idx =~ s#/keys.est##;
    my $longcat = $longcat{$idx};
    unless ($longcat) {
	$longcat = $idx;
	$longcat =~ s#^02pub/cbd(.*?)/keys.est#$1#;
    }
    my $tit = "$project\'s $longcat index";
    my $uri = "$project/search/$idx";
    my $est = "01tmp/$idx.est";
    print "draft doc from $in saved to $est with uri $uri\n";
    open(E, ">$est");
    print E "\@title=$tit\n";
    print E "\@type=text/plain\n";
    print E "\@uri=$uri\n";
    print E "category=$cat\n\n";
    close E;
    system ("cut -f1 <$in >>$est");
}

1;
