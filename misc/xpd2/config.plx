#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XPD::Util;
ORACC::XPD::Util::load_xpd();
print ORACC::XPD::Util::option($ARGV[0]);
1;
