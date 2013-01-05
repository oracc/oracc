#!/usr/bin/perl
use lib "$ENV{'ORACC'}/lib";
use ORACC::XPD::Util;

my $tei_list = ORACC::XPD::Util::option('build-tei-list');
my $tei_trim = ORACC::XPD::Util::option('build-tei-trim');
my $tei_dist = ORACC::XPD::Util::option('build-tei-dist');
my $tei_xml = undef;

my $project = `oraccopt`;
my $tei_list_file = undef;
my $xmlfile = undef;

if ($tei_list) {
    if ($tei_list eq 'atf') {
	$tei_list_file = '01bld/lists/have-atf.lst';
    } elsif ($tei_list eq 'lem') {
	$tei_list_file = '01bld/lists/lemindex.lst';
    } elsif ($tei_list eq 'all') {
	$tei_list_file = '01bld/lists/xtfindex..lst';
    } else {
	die "teimanager.plx: value `$tei_list' invalid for option `build-tei-list'\n";
    }
} else {
    $tei_list_file = '01bld/lists/have-atf.lst';
}

system 'tei-project.sh', $project, $tei_list_file;

if ($tei_trim && $tei_trim eq 'yes') {
    $xmlfile = `tei-trim.sh $project`;
} else {
    $xmlfile = `tei-no-trim.sh $project`;
}

if ($tei_dist && $tei_dist eq 'yes') {
    system 'tei-dist.sh', $xmlfile;
}

1;
