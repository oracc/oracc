#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
use Getopt::Long;

# Add a '#project: ' line to each text in the files given
# on the command line.  Project name is specified with 
# `-project' flag.

# If `-legacy' is given, add the `#atf: use legacy' 
# protocol as well.

my @files = ();
my $legacy = 0;
my @lines = ();
my $project = '';
my $replace = 0;
my $stdin = 0;
my $unicode = 0;

GetOptions (
    'legacy+'=>\$legacy,
    'project:s'=>\$project,
    'replace'=>\$replace,
    'stdin'=>\$stdin,
    'unicode+'=>\$unicode,
    );

die "atfproject.plx: must give -project <PROJECT> on command line\n"
    unless $project;

unless ($stdin) {
    @files = @ARGV;
    if ($#files < 0) {
	@files = <00atf/*.atf>;
    }
}

if ($#files >= 0) {

    foreach my $f (@files) {
	open(F,"$f");
	my @lines = (<F>);
	close(F);
	projectify($f, @lines);
    }
} elsif ($stdin) {
    @lines = (<>);
    projectify('', @lines);
} else {
    die "atfproject.plx: no files specified and -stdin not given\n";
}

sub
projectify {
    my ($f, @lines) = @_;
    if ($f) {
	warn "atfproject.plx: can't open `$f' for output\n" and next unless open(F,">$f");
	select F;
    }
    for (my $i = 0; $i <= $#lines; ++$i) {
	if ($lines[$i] =~ /^\x{feff}?\&/) {
	    my $proj = -1;
	    for (my $j = $i+1; $j <= $#lines; ++$j) {
		next if $lines[$j] =~ /^\s*$/;
		$proj = $j and last if $lines[$j] =~ /^\#project:/;
		last unless $lines[$j] =~ /^\#/;
	    }
	    print $lines[$i];
	    if ($lines[$i+1] =~ /^\@(composite|score)/) {
		print $lines[++$i];
	    }
	    if ($proj == -1 || $replace) {
		print "#project: $project\n";
	    } else {
		print $lines[$proj];
	    }
	    print "#atf: use legacy\n" if $legacy;
	    print "#atf: use unicode\n" if $unicode;
	} else {
	    print $lines[$i] unless $lines[$i] =~ m/^#project/;
	}
    }
    close(F) if $f;
}

1;
