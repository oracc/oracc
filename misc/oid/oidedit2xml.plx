#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";

use ORACC::OID;
use ORACC::XML;

print '<editoid>';

while (<>) {
    chomp;
    my @f = split(/\t/,$_);
    next if $f[2] =~  m#//#; # ignore sense changes for now
    my($oid,$cgp,$type) = ($f[1],$f[2],$f[3]);
    if ($type eq '+') {
	print '<add day="'.$f[0].'" oid="'.$oid.'" cgp="'.xmlify($cgp).'"'."\n/>";
    } elsif ($type eq '-') {
	my $why = $f[4];
	my $whyattr = '';
	if ($why) {
	    my $whycgp = '';
	    if ($why =~ /\[\[\s*(.*?)\s*\]\]/) {
		$whycgp = $1;
	    } elsif ($why =~ /(\S+\[(.*?)\]\S+)/) {
		$whycgp = $1;
		$whycgp =~ tr/_/ /; # compounds in #why must be, e.g., a_sa[draw water]V/t
	    }
	    if ($whycgp) {
		my $whyoid = oid_lookup('sux',$whycgp);
		if ($whyoid) {
		    $whyattr = " why-cgp=\"$whycgp\" why-oid=\"$whyoid\"";
		} else {
		    warn "$0: oid_lookup failed on $whycgp\n";
		}
	    }
	}
	print '<del day="'.$f[0].'" oid="'.$oid.'" cgp="'.xmlify($cgp).'" why="'.xmlify($why).'"'.$whyattr."\n/>";
    } elsif ($type eq '=') {
	my($nowoid,$nowcgp) = ($f[4],$f[5]);
	print '<mrg day="'.$f[0].'" oid="'.$oid.'" cgp="'.xmlify($cgp).'" now-oid="'.$nowoid.'" now-cgp="'.xmlify($nowcgp).'"'."\n/>";
    } elsif ($type eq '>') {
	my $nowcgp = $f[4];
	print '<ren day="'.$f[0].'" oid="'.$oid.'" cgp="'.xmlify($cgp).'" now-cgp="'.xmlify($nowcgp).'"'."\n/>";
    } else {
	warn "$0: unhandled type $type\n";
    }
}

print '</editoid>';
1;
