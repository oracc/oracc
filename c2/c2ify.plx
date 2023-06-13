#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my @c = <*.c>;
my @i = <*.in>;
my @h = <*.h>;

my %i = (); @i{@i} = ();

system 'rm', '-rf', 'new';
system 'mkdir', '-p', 'new';

foreach my $f (@h, @i, @c) {
    next if exists $i{"$f.in"};
    my @f = `cat $f`;
    foreach (@f) {
	s/Hash_table/Hash/g;
	s/npool/pool/g;
	s/struct pool/Pool/g;
	s/mb_/memo_/g;
	s/struct memblock/Memo/g;
	s/memblock.h/memo.h/;
	s/warning.h/mesg.h/;
	s/vwarning2/mesg_vwarning/g;
	s/lang.h/lng.h/;
    }
    open(N,">new/$f") || die;
    print N @f;
    close(N);
}

1;
