#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Hash;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;

my $e = undef;
my $h = undef;

my $fixfile = shift @ARGV;
die "$0: must give fixes file on command line \n" unless $fixfile;
die "$0: no $fixfile\n" unless -r  $fixfile;
my $lang = $fixfile; $lang =~ s/\..*$//;
my $glofile = "00lib/$lang.glo";
die "$0: no $glofile\n" unless -r $glofile;

my @from_entries = `cbdentries.plx $glofile`; chomp @from_entries;
die unless $#from_entries >= 0;
my %from_entries = (); @from_entries{@from_entries} = ();

my %fixes = ();
open(F,$fixfile) || die "$0: can't open $fixfile for read\n";
while (<F>) {
    /^(.*?)\t(.*?)$/;
    my($from,$to) = ($1,$2);
    if ($from eq $to) {
	warn "$0: ignoring fix '$from' == '$to'\n";
    } elsif (exists $from_entries{$from}) {
	$fixes{$from} = $to;
    } else {
	warn "$0: entry '$from' not in $glofile\n";
    }
}

# Extract the entries we are going to edit from $glofile

#open(S,'>01tmp/subset.lst');
#foreach my $k (keys %fixes) {
#    print S "$k\n";
#}
#close(S);
#system "cbdsubset.plx $glofile <01tmp/subset.lst >01tmp/subset.glo";

# Create a version of $glofile without the entries we just extracted
#system "cbdsubset.plx -i $glofile <01tmp/subset.lst >01tmp/stripped.glo";

# Edit the $glofile subset
#open(S,'01tmp/subset.glo');
open(S,$glofile);
open(N,'>01tmp/edited.glo');
while (<S>) {
    if (/(\@entry.*?)\s+(.*?)\s*$/) {
	my ($e,$cfgw) = ($1,$2);
	if ($fixes{$cfgw}) {
	    $_ = "$e $fixes{$cfgw}\n";
	}
    } elsif (s/\@parts\s+//) {
	my @p = '@parts';
	while (s/^\s*(.*?\]\S+)//) {
	    my $p = $1;
	    my $save = $p;
	    $p =~ s/\[(.*?)\]/ \[$1\] /;
	    if ($fixes{$p}) {
		my $f = $fixes{$p};
		$f =~ s/\s+\[(.*?)\]\s+/\[$1\]/;
		push @p, $f;
	    } else {
		push @p, $save;
	    }
	}
	$_ = join(' ', @p)."\n";
    } elsif (/\@bff/) {
	if (/<(.*?)>/) {
	    my $ref = $1;
	    if ($fixes{$ref}) {
		s/<(.*?)>/<$fixes{$ref}>/;
	    }
	}
    }
    print N;
}
close(N);
close(S);

# And merge the edited subset from the stripped version of $glofile
#system "cbdmerge.plx -base 01tmp/stripped.glo 01tmp/edited.glo >01tmp/new.glo";

1;
