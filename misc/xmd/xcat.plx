#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $has_atf = `condfind.plx 00atf atf`;
unless ($has_atf) {
#    warn "xcat.plx: no atf files\n";
    exit 0;
}

open(X,"grep -r '\&X[0-9]' 00atf |");
my @X = (<X>);
close(X);
my %X = ();
foreach my $X (@X) {
    next unless $X =~ /\.atf:/;
    my($XID,$XNM) = ($X =~ /(?<!\#)\&(X\d+)(?:\s*=\s*(.*))?\s*$/);
    next unless defined $XID;
    warn "xcat: XID $XID has no name\n" unless $XNM;
    $XID =~ s/^\s*(\S+)\s*$/$1/;
    $XNM =~ s/^\s*(.*?)\s*$/$1/;
    $XNM =~ s/\"/&quot;/g;
    $XNM =~ s/\&/&amp;/g;
    $XNM =~ s/\</&lt;/g;
    $X{$XID} = $XNM;
}

if (scalar keys %X > 0) {
    open(X,">01bld/cat/dynamic-x.xml");
    print X '<catalog xmlns="http://oracc.org/ns/xmd/1.0" xmlns:xmd="http://oracc.org/ns/xmd/1.0">';
    foreach my $X (sort keys %X) {
	print X
	    "<record><id_text>$X</id_text><designation>$X{$X}</designation></record>";
    }
    print X '</catalog>';
    close(X);
}

1;
