#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XPD::Util;

my @atf = <00atf/*.atf>;
my @glo = <00lib/*.glo>;
my @dep = ();

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
	push @glo, "$ENV{'ORACC_BUILDS'}/$proj/02pub/lemm-$gl.sig";
    }
}

my @nglo = ();
foreach my $g (@glo) {
    if (-r $g) {
	push @nglo, $g;
    # } else {
    # 	my $g2 = $g;
    # 	$g2 =~ s/00lib/00src/;
    # 	if (-r $g2) {
    # 	    push(@nglo, $g2);
    # 	} else {
    # 	    $g =~ s#/[^/]+\.glo$#/qpn.glo#;
    # 	    push(@nglo,$g) if -r $g;
    # 	}
    }
}

open(M,'>00lib/Makefile');
print M "ATF=@atf\n\nLEM=@nglo\n\n01bld/buildstamp: \$\{ATF} \$\{LEM}\n\toracc build\n";
close(M);

1;
