#!/usr/bin/perl
use warnings; use strict;
use Getopt::Long;

if ($ENV{'ORACC_MODE'} ne 'single') {
    my $g = `id -Gn`;
    exit 1 unless $g =~ /oracc/;
}

my $list;
my $project;

GetOptions(
    "list:s"=>\$list,
    "project:s"=>\$project,
    );

die "agg-proxify.plx: must give -list argument\n" unless $list;
die "agg-proxify.plx: list `$list' not readable\n" unless -r $list;
die "agg-proxify.plx: must give -project argument\n" unless $project;

open(L, $list) || die "$!";
my @pqx = (<L>); chomp @pqx;
close(L);

my %pqx = (); @pqx{@pqx} = ();

my $master_xtf = '/home/oracc/agg/master-xtf.lst';

open(M, $master_xtf) || die "agg-proxify.plx: can't open $master_xtf\n";
my @m = (<M>); chomp @m;
close(M);

foreach my $m (sort @m) {
    my($p) = ($m =~ /:(.*?)$/);
    if (exists $pqx{$p}) {
	$pqx{$p} = $m;
    }
}

foreach my $n (keys %pqx) {
    $pqx{$n} = "$project\:$n" unless defined $pqx{$n};
}

print join("\n", sort grep(defined,values %pqx)), "\n";

1;
