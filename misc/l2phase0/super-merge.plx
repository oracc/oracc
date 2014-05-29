#!/usr/bin/perl
use warnings; use strict;
binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Builtins;
use Data::Dumper;

use POSIX qw(strftime);
my $isodate = strftime("%Y%m%d", gmtime());

$ORACC::L2GLO::Builtins::bare = 1;

my @glo = <00lib/*.glo>;

die "super compare: a super-glossary must have a .glo file\n" if $#glo < 0;
die "super compare: a super-glossary is only allowed one .glo file\n" if $#glo > 0;

my $base = shift @glo;
my $baselang = $base; $baselang =~ s#^.*?/(.*?)\.glo$#$1#;
my $out = $base; $out =~ s/00lib/01tmp/;
my $globak = $base; $globak =~ s/00lib/00bak/; $globak =~ s/\./-$isodate./;

my $new = shift @ARGV;
my $mapfile = $new; $mapfile =~ s/new$/map/;
my $srcmap = $mapfile; $srcmap =~ s/01tmp/00map/;
my $mapbak = $srcmap; $mapbak =~ s/00map/00bak/; $mapbak =~ s/\./-$isodate./;

# before merging, make sure the new glossary file is later in date than the current map
# file, otherwise changes to the map file may be lost

my $mapdate = (stat($srcmap))[9];
my $newdate = (stat($new))[9];

if ($newdate < $mapdate) {
    die <<EOD;
super merge: can\'t merge when $new is older than $srcmap.
If you are sure you want to merge, say:

	touch $new

and then run the merge again.
EOD
}

ORACC::L2GLO::Builtins::merge_exec($baselang, $base, [$new], $out);

warn "super merge: backing up $base to $globak\n";
system "mv $base $globak";
warn "super merge: installing $out as new version of $base\n";
system "mv $out $base";

warn "super merge: backing up $srcmap to $mapbak\n";
system "mv $srcmap $mapbak";
warn "super merge: installing $mapfile as new version of $srcmap\n";
system "mv $mapfile $srcmap";

1;
