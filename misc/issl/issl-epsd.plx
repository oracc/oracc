#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
my %ks = ();
my %t = ();
open(EPSD,'00src/epsd.raw') || die "epsd-issl.plx: no 00src/epsd.raw\n";
while (<EPSD>) {
    my($t,$publ,$loc) = /^(.*?)\s+::\s+::\s+::\s+(ePSD)\s+::\s+(.*)$/;
    my $eid = $loc;
    $eid =~ s/^\*//; $eid =~ s/\s.*$//;
    push @{$t{$eid}}, $t;
    push @{$ks{$t}},{ year=>'',auth=>'',publ=>$publ,loc=>$loc };
}
close(EPSD);
#
# This used to have additional refs from a writing to the ISSLp, but it's
# redundant because ISSLp can be reached from the ePSD articles, which are
# reachable because all writings of each entry are indexed.
#
# open(PRIME,'sources/prime.raw') || die "epsd-issl.plx: no sources/prime.raw\n";
# while (<PRIME>) {
#     my($t,$year,$auth,$publ,$loc) 
# 	= /^(.*?)\s+::\s+(.*?)\s+::\s+(.*?)\s+::\s+(.*?)\s+::\s+(.*?)$/;
#     die unless $t;
#     my($eid,$item) = ($t =~ /^(.*?)\/(.*?)$/);
#     $item = '' unless $item;
#     my $ref = { year=>$year,auth=>$auth,publ=>$publ,loc=>$loc };
#     push(@{$ks{$item}}, $ref) if $item;
#     foreach my $t (@{$t{$eid}}) {
# 	push(@{$ks{$t}}, $ref) unless $t eq $item;
#     }
# }
# close(PRIME);
open(KS,">00src/epsd+prime.txt") || die "epsd-issl.plx: can't write sources/epsd+prime.txt\n";
foreach my $e (keys %ks) {
    foreach my $r (@{$ks{$e}}) {
	print KS "$e :: $$r{'year'} :: $$r{'auth'} :: $$r{'publ'} :: $$r{'loc'}\n";
    }
}
close(KS);
1;
