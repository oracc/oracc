#!/usr/bin/perl
use warnings; use strict;
use open 'utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::NS;
use Getopt::Long;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $last_good = '';
my %map = ();
my %sig = ();
my %simple = ();

my($sig,$map) = @ARGV;

open(M,$map) || die "$0: can't open map $map\n";
while (<M>) {
    chomp;
    if (s/^\t//) {
	$map{$_} = $last_good;
    } else {
	$last_good = $_;
    }
}
close(M);

open(S,$sig) || die "$0: can't open sigs $sig\n";
my $header = <S>;
while (<S>) {
    next if /^\s*$/;
    s/^\!//; # notation for base-merge not needed here
    chomp;
    my($sig,$inst) = split(/\t/,$_);
    my($pre,$core,$post) = ($sig =~ /^(.*?)=(.*?)\$(.*?)$/);
    if ($map{$core}) {
	my $cf = $core;
	$cf =~ s/\[.*$//;
	my $ncf = $map{$core};
	$ncf =~ s/\[.*$//;
	$post =~ s/^$cf/$ncf/;
	$sig = "$pre=$map{$core}\$$post";
#	warn "output sig=$sig\n";
    }
    push @{$sig{$sig}}, $inst;
}
close(S);

open(N,">$sig") || die "$0: can't write to $sig\n"; select N;
print STDERR "$0: writing remapped $sig\n";
print $header, "\n";
foreach my $s (sort keys %sig) {
    print "$s\t", join(' ', @{$sig{$s}}), "\n";
}
close(N);
1;
