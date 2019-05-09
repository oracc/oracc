#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDERR, ':utf8'; binmode STDOUT, ':utf8';

use Encode;
use lib "$ENV{'ORACC_BUILDS'}/lib/";
use ORACC::OID;
use ORACC::XML;

my %wid2err = ();
open(W,"$ENV{'ORACC_BUILDS'}/dcclt/01tmp/wid2err.tab") || die;
while (<W>) {
    chomp;
    /^(.*?)\t(.*?)$/;
    $wid2err{$1} = $2;
}
close(W);

$ORACC::OID::verbose = 0;

my %badsigns = ();
my $xid = 'x0000000';
oid_init('sl');

my $master = load_xml("sign-master.xml");
foreach (tags($master, 'http://oracc.org/ns/lex/1.0', 'data')) {
    my $signattr = $_->getAttribute('sign');
    next unless $signattr;
    my @signs = split(/\s+/, $signattr);
    my @oids = ();
    foreach my $sign (@signs) {
	$sign =~ s/\{.*?\}//g; # ignore determinatives
	$sign =~ s/×\|$/|/; $sign =~ s/×\(\)\|$/|/; # fix a few rare writings
	my $oid = oid_lookup('sl', $sign);
	if ($oid) {
	    push @oids, $oid;
#	    $_->setAttribute('oid', $oid);
	} else {
	    $badsigns{$sign} = ++$xid unless $badsigns{$sign};
	    next if ignore_sign($sign);
	    my $proj = $_->getAttribute('project');
	    my $sref = $_->getAttribute('sref');
	    my $werr = $wid2err{$sref};
	    push @oids, $badsigns{$sign};
#	    $_->setAttribute('oid', $badsigns{$sign});
	    warn "$ENV{'ORACC_BUILDS'}/$proj/$werr: no OID for sign $sign\n";
	}
    }
    $_->setAttribute('oid', "@oids");
}

my $s = $master->toString();
Encode::_utf8_on($s);
open(X,'>sign-master.xml'); print X $s; close(X);

sub ignore_sign {
    my $t = shift;
    $t =~ tr/-.xX//d;
#    print "t = $t\n";
    length $t == 0;
}

#foreach my $bad (sort keys %badsigns) {
#    warn "$0: no sl OID for sign $bad\n";
#}

1;
