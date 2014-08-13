#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use Getopt::Long;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $all = 1;
my $first = 1;
my %header = ();
my $lang = '';
my $quiet = 0;
my $project = '';
my %sig = ();
my $superglo = 0;

GetOptions(
    'lang:s'=>\$lang,
    'project:s'=>\$project,
    'quiet'=>\$quiet,
    'super'=>\$superglo,
    );

## No, we have to move this downstream to prevent failed resolution of parts of PSUs
#my $withall = `oraccopt . cbd-with-all`;
#$all = 1 if $withall && $withall eq 'yes';

foreach my $s (@ARGV) {
    unless (open(S,$s)) {
	warn "l2-sig-union.plx: no sig file $s\n"
	    unless $quiet;
	next;
    }
    while (<S>) {
	chomp;
	next if /^\s*$/;
	if (/^\@(project|name|lang)\s+(.*?)$/) {
	    $header{$1} = $2;
	    next;
	}
	my @t = split(/\t/, $_);
	my $r = (($#t == 2) ? $t[2] : (($#t == 1) ? $t[1] : ''));
	if ($superglo) {
	    # in a superglo we read the glossary sigs first, then only allow in
	    # the ones that have been vetted into the main superglo file
	    if ($first || $sig{$t[0]}) {
		my @r = split(/\s/, $r);
		if ($all || $#r >= 0) {
		    @{$sig{$t[0]}}{@r} = ();
		}
	    }
	} else {
	    my @r = split(/\s/, $r);
	    if ($all || $#r >= 0) {
		@{$sig{$t[0]}}{@r} = ();
	    }
	}
    }
    close(S);
    $first = 0;
}

$header{'project'} = $project unless $header{'project'};
$header{'lang'} = $lang unless $header{'lang'};
$header{'name'} = "$project $lang" unless $header{'name'};

foreach my $k (qw/project name lang/) {
    print "\@$k $header{$k}\n";
}
print "\n";

#use Data::Dumper; open(X,">dump.log");
#print X Dumper \%sig;
#close(X);

foreach my $s (sort keys %sig) {
    print "$s\t";
    my @r = uniq(keys %{$sig{$s}});
    if ($#r >= 0) {
	printf "%d\t@r", $#r+1;
    } else {
	print "\t0";
    }
    print "\n";
}

sub
uniq {
    my %x = ();
    foreach (grep($_, @_)) {
	@x{grep($_, split(/\s+/,$_))} = ();
    }
    sort keys %x;
}

1;
