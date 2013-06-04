#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use lib '/usr/local/oracc/lib';
use ORACC::SE::DBM;
use ORACC::SE::XML;
use NDBM_File;
use Encode;
use Fcntl;

my $db = shift @ARGV;
ORACC::SE::DBM::create($db);

1;
