#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XPD::Util;
use Getopt::Long;

my $driver = '';
my $list = '';
my $preview = 0;
my $project = '';

GetOptions(
    'driver:s'=>\$driver,
    'list:s'=>\$list,
    'preview'=>\$preview,
    'project:s'=>\$project,
);

my @args = ();
$project = `oraccopt` unless $project;
push(@args, "-proj $project") if $project;
push(@args, "-list $list") if $list;
unless ($driver) {
    if (-r '00lib/project.otf') {
	$driver = '00lib/project.otf';
    } else {
	$driver = `otfmanager.plx @args -d @ARGV`;
    }
}
chomp $driver;
if ($preview) {
    system 'otf2tex.sh', $driver;
    my $pdf = $driver;
    $pdf =~ s/otf$/pdf/;
    $pdf =~ s#01tmp/.*?/#01tmp/pdf/#;
    warn "evince $pdf\n";
    system "evince $pdf &";
    exit 0;
} else {
    exec 'otf2tex.sh', $driver;
}
