#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
my %args = @ARGV;
$args{'date'} = `date -u +\%FT\%TZ`; chomp $args{'date'};
my %funcs = (
    GetRecord=>\&GetRecord,
    Identify=>\&Identify,
    ListIdentifiers=>\&ListIdentifiers,
    ListMetadataFormats=>\&ListMetadataFormats,
    ListRecords=>\&ListRecords,
    ListSets=>\&ListSets,
    );

foreach my $a (sort keys %args) {
    print "$a => $args{$a}\n";
}

#if ($args{'verb'}) {
#    if ($funcs{$args{'verb'}}) {
#	&$funcs{$args{'verb'}};
#    } else {
#	oai_error();
#    }
#} else {
#    oai_error();
#}

1;
