#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use File::Basename;
my %names = ();
my $oracc = $ENV{'ORACC'};
my $project = `pwd`; chomp $project; 
($project) = basename($project);
my @atfs = `grep -r '^&' 00atf`;
foreach my $a (@atfs) {
    next if $a =~ /~:/;
    my($p,$n) = ($a =~ /^.*?:.(.*?)\s*=\s*(\S.*)\s*$/);
    my $orig_n = $n;
    $n =~ s/\s+/_/g;
    $n = "\L$n";
    if ($names{$n}) {
	warn "atfmap: duplicate \&-name '$orig_n',skipping $p (already used for $names{$n})\n";
	next;
    } else {
	$names{$n} = $p;
    }
    system 'ln', '-sf', "$oracc/$project/00atf/$p.atf", "$oracc/$project/00map/$n.atf";
}

1;
