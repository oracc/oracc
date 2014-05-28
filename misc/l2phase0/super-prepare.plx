#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Builtins;

$ORACC::L2GLO::Builtins::bare = 1;

my $mapfile = shift @ARGV;
my $glofile = $mapfile;
my $newmap = $mapfile;
my $srcfile = $mapfile;

$glofile =~ s/map$/new/; $glofile =~ s/00map/01tmp/;

$newmap =~ s/00map/01tmp/;

$srcfile =~ s/map$/glo/; $srcfile =~ s/00map/00src/;

my $srcdata = ORACC::L2GLO::Builtins::input_acd($srcfile);
my %srchash = %{$$srcdata{'ehash'}};

open(M,$mapfile) || die "super prepare: unable to read map file $mapfile. Stop\n";
open(N, ">$newmap") || die "super prepare: unable to write new map file $newmap. Stop\n";
open(G,">$glofile") || die "super prepare: unable to write glo file $glofile. Stop\n";

my @map = ();
my %glo = ();

while (<M>) {
    if (/^add/) {
	my ($cfgwpos,@senses) = parse_map($_);
	if ($cfgwpos) {
	    if ($#senses >= 0) {
		foreach my $s (@senses) {
		    push @{$glo{$cfgwpos}}, [ scalar keys %glo, $s ];
		}
	    } else {
		unless ($glo{$cfgwpos}) { # silently ignore add entry that comes after add sense for same word
		    @{$glo{$cfgwpos}} = ();
		}
	    }
	}
    } else {
	push @map, $_;
    }
}
close(M);

print N @map;
close(N);

foreach my $e (sort { ${${$glo{$a}}[0]}[0] <=> ${${$glo{$b}}[0]}[0] } keys %glo) {
    print G "\@entry $e\n";
    foreach my $s (sort { $$a[1] cmp $$b[1] } @{$glo{$e}}) {
	print G "\@sense $$s[1]\n";
    }
    print G "\@end entry\n\n";
}

close(G);



##############################################################################

sub
parse_map {
    chomp;
    if (s/^add\s+entry\s+//) {
	s/\s*$//;
	if ($srchash{$_}) {
	    my %e = %${$srchash{$_}};
	    s/\[/ [/; s/\]/] /;
	    my @parts = ();
	    @parts = @{$e{'parts'}} if defined $e{'parts'};
	    return ($_, @parts, @{$e{'sense'}})
	} else {
	    warn "super prepare: entry $_ not found in source glossary $srcfile\n";
	    return ();
	}
    } else {
	s/^add\s+sense\s+//;
	my($cf,$gw,$sense,$pos,$epos) = (m#^(.*?)\[(.*?)//(.*?)\](.*?)\'(.*?)$#);
	return ("$cf [$gw] $pos", "$epos $sense");
    }
}

1;
