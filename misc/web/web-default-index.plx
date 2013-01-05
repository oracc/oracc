#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XPD::Util;

my $webdir = shift @ARGV;
die "web-default-index.plx: must give webdir on command line\n"
    unless $webdir;

my $opt = ORACC::XPD::Util::option('build-default-index');
if (!$opt || $opt eq 'yes') {
    exec 'xsltproc', 
    '-o', "$webdir/index.html",
    '--stringparam', 'HOST', $ENV{'ORACC_HOST'},
    "$ENV{'ORACC'}/lib/scripts/web-default-index.xsl",
    '00lib/config.xml';
}

1;
