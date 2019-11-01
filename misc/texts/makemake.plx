#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XPD::Util;

my @atf = <00atf/*.atf>;
my @glo = <00lib/*.glo>;
my @lem = ();

ORACC::XPD::Util::load_xpd();
my @l = ORACC::XPD::Util::lang_options();

foreach my $l (@l) {
    my $lv = ORACC::XPD::Util::option($l);
#    warn "$l = $lv\n";
    my $gl = $l; $gl =~ s/^%//;
    my $proj = '';
    foreach my $g (split(/\s+/, $lv)) {
	next if $g eq '.';
	if ($g =~ /^(.*?):(.*?)$/) {
	    ($proj,$gl) = ($1,$2);
	} else {
	    $proj = $l;
	}
	push @lem, "$ENV{'ORACC_BUILDS'}/$proj/02pub/lemm-$gl.sig";
    }
}

my @nglo = ();
foreach my $g (@glo) {
    if (-r $g) {
	push @nglo, $g;
    }
}

my @nlem = ();
foreach my $l (@lem) {
    if (-r $l) {
	push @nlem, $l;
    }
}

open(M,'>00lib/Makefile');
print M "ATF=@atf\n\nGLO=@nglo\n\nLEM=@nlem\n\n";
print M "01bld/buildstamp: \$\{ATF} \$\{GLO} \$\{LEM}\n\toracc build\n\n";
print M "01bld/updatestamp: \$\{ATF} \$\{GLO} \$\{LEM}\n\toracc update\n\ttouch 01bld/updatestamp\n\n";
foreach my $g (@nglo) {
    my $l = $g;
    $l =~ s#00lib/(.*?)\.glo$#$1#;
    print M "02pub/lemm-$l.sig: $g\n\toracc update\n\n";
}
close(M);

1;
