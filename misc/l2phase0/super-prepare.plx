#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Builtins;

$ORACC::L2GLO::Builtins::bare = 1;

my $chatty = 1;
my $mapfile = shift @ARGV;
my $glofile = $mapfile;
my $newmap = $mapfile;
my $srcfile = $mapfile;

$glofile =~ s/map$/new/; $glofile =~ s/00map/01tmp/;
$newmap =~ s/00map/01tmp/;
$srcfile =~ s/map$/glo/; $srcfile =~ s/00map/00src/;

my $srcdata = ORACC::L2GLO::Builtins::input_acd($srcfile);
#use Data::Dumper; print Dumper $srcdata;
my %srchash = %{$$srcdata{'ehash'}};

open(M,$mapfile) || die "super prepare: unable to read map file $mapfile. Stop\n";
open(G,">$glofile") || die "super prepare: unable to write glo file $glofile. Stop\n";
open(N, ">$newmap") || die "super prepare: unable to write new map file $newmap. Stop\n";

my @map = ();
my %glo = ();

my($mapref,$gloref) = ORACC::L2P0::L2Super::parse_mapfile($mapfile);
my @map = @$mapref;
my %glo = %$gloref;

print N @map;
close(N);

foreach my $e (sort { ${${$glo{$a}}[0]}[0] <=> ${${$glo{$b}}[0]}[0] } keys %glo) {
    print G "\@entry $e\n";
    my @econtent = @{$glo{$e}};
    if (${$econtent[0]}[1] =~ /^\@parts/) {
	my $p = shift @econtent;
	print G $$p[1];
    }
    foreach my $s (sort { $$a[1] cmp $$b[1] } @econtent) {
	print G "\@sense $$s[1]\n";
    }
    print G "\@end entry\n\n";
}

close(G);

chatty("super prepare: additions for base are in $glofile");
chatty("super prepare: new version of map file is in $newmap");

1;
