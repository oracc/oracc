#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XPD::Util;
use Getopt::Long;

my $list = '';
my $preview = 0;
my $project = '';

GetOptions(
    'list:s'=>\$list,
    'preview'=>\$preview,
    'project:s'=>\$project,
);

my @args = ();
$project = `oraccopt` unless $project;
push(@args, "-proj $project") if $project;
push(@args, "-list $list") if $list;
my $driver = `odtmanager.plx @args -d @ARGV`;
chomp $driver;
if ($preview) {
    system 'cdf2tex.sh', $driver, $project;
    my $pdf = $driver;
    $pdf =~ s/cdf$/pdf/;
    warn "evince $pdf\n";
    system "evince $pdf &";
    exit 0;
} else {
    exec 'cdf2tex.sh', $driver, $project;
}
