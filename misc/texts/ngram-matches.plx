#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDERR, ':utf8'; binmode STDOUT, ':utf8';
use lib '@@ORACC@@/lib';
use Getopt::Long;
use Data::Dumper;

my $type = shift @ARGV;

my %d = ();
my %l = ();

while (<>) {
    chomp;
    my ($type,$ngm_fl,$atf_fl,$url,$ngm_line) = split(/\t/,$_);
    my ($nf,$nl) = ($ngm_fl =~ /^(.*?):(.*)$/);
    if ($nf && $nl) {
	push @{${$d{$nf}}{$nl}}, [ $atf_fl, $url ];
	$l{$ngm_fl} = $ngm_line;
    }
}

foreach my $f (keys %d) {
    unless (-r $f) {
	warn "$0: unable to read collo file $f\n";
	next;
    }
    my @src = `cat $f`; chomp @src;
    print "$f\t";
    foreach my $l (sort { $a <=> $b } keys %{$d{$f}}) {
	my $nfl = "$f:$l";
	print "$l ";
	if ($src[$l-1] eq $l{$nfl}) {
	    my $t = sprintf("%d\t", $#{${$d{$f}}{$l}}+1);
	    $src[$l-1] =~ s/^/$t/;
	} else {
	    warn "$0: [f=$f;l=$l] $src[$l-1] ne $l{$nfl}\n";
	}
    }
    print "\n";
    @src = map { /\t/ || s/^/0\t/; $_ } @src;
    open(S,">01tmp/$type-match.log"); print S join("\n", @src, ''); close(S);
}

1;
