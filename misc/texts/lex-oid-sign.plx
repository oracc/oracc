#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
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

$ORACC::OID::verbose = 1;

my %badsigns = ();
my $xid = 'x0000000';
oid_init('sl');

my $master = load_xml("sign-master.xml");
foreach (tags($master, 'http://oracc.org/ns/lex/1.0', 'data')) {
    my $sign = $_->getAttribute('sign');
    next unless $sign;
    my $oid = oid_lookup('sl', $sign);
    if ($oid) {
	$_->setAttribute('oid', $oid);
    } else {
	$badsigns{$sign} = ++$xid unless $badsigns{$sign};
	my $proj = $_->getAttribute('project');
	my $sref = $_->getAttribute('sref');
	my $werr = $wid2err{$sref};
	$_->setAttribute('oid', $badsigns{$sign});
	warn "$ENV{'ORACC_BUILDS'}/$proj/$werr: no OID for sign $sign\n";
    }
}

my $s = $master->toString();
Encode::_utf8_on($s);
open(X,'>sign-master.xml'); print X $s; close(X);

#foreach my $bad (sort keys %badsigns) {
#    warn "$0: no sl OID for sign $bad\n";
#}

1;
