#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2P0::L2Super;

#
# Prepare a map file for merging: pull out the 'add' entries
# and turn them into a glossary, and write a new map which has
# the non-add entries in it.  Do it all in 01tmp so that it
# can be reviewed before the merge goes ahead
#

my %data = ORACC::L2P0::L2Super::init();

# map_dump is already done in the init routine.

my %glo = %{$data{'map_glo'}};
my $outfh = $data{'outglo_fh'}; select $outfh;
foreach my $e (sort { ${${$glo{$a}}[0]}[0] <=> ${${$glo{$b}}[0]}[0] } keys %glo) {
    print "\@entry $e\n";
    my @econtent = @{$glo{$e}};
    if (${$econtent[0]}[1] =~ /^\@parts/) {
	my $p = shift @econtent;
	print $$p[1];
    }
    foreach my $s (sort { $$a[1] cmp $$b[1] } @econtent) {
	print "\@sense $$s[1]\n";
    }
    print "\@end entry\n\n";
}
close($outfh);

chatty("super prepare: additions for base are in $data{'outglo'}");
chatty("super prepare: new version of map file is in $data{'outmap'}");

1;
