#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::Legacy::StyleInfo;
use Getopt::Long;

my $odf = '';
GetOptions(
    );
$odf = shift @ARGV;
die "style-info.plx: can't proceed without an ODF file\n"
    unless $odf;
die "style-info.plx: ODF file `$odf' non-existent or unreadable\n"
    unless -r $odf;

ORACC::Legacy::StyleInfo::loadinfo($odf);

ORACC::Legacy::StyleInfo::showinfo();

1;
