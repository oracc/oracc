#!/usr/bin/perl
use warnings; use strict; use open 'utf8';

binmode STDOUT, 'utf8';

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::XML;

my $f = shift @ARGV;
my $x = load_xml($f);

my @forms = tags($x,'http://oracc.org/ns/cbd/1.0','form');
foreach my $f (@forms) {
    my @rs = tags($f,'http://oracc.org/ns/xis/1.0','r');
    my $last_vol = '';
    foreach my $r (@rs) {
	my $l2 = $r->getAttribute('label2');
	if ($l2 && $l2 =~ /^(.*?)_/) {
	    my $v = $1;
	    if ($v eq $last_vol) {
		$l2 =~ s/^.*?_//;
		$r->setAttribute('label2', $l2);
	    } else {
		$last_vol = $v;
	    }
	}
    }
}

$f =~ s/\./+./;
open(X,">$f");
print X $x->toString();
close(X);

1;
