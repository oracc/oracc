#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2P0::L2Super;
use ORACC::L2GLO::Util;

#
# Compare two glossaries and output a map of the differences.
# 
# By default, operate on a single source glossary which is compared
# against a superglossary's base glossary.
#
# By option, use -b to set the base, -c to set the comparand and
# -m to name the resultant map file (without a -m option the map
# is sent to STDOUT.
#

my %map = ();
my %sort = ();
my @warnings = ();

my %data = ORACC::L2P0::L2Super::init();

chatty("using base glossary = $data{'basefile'}");
chatty("using comparison glossary = $data{'srcfile'}");
glo_compare();
chatty("writing map file $data{'mapfile'}")
    if ($data{'mapfile'};
map_dump();
map_unload();

chatty("done.");

##################################################################################

1;
