#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
use Getopt::Long;

# Add a '#project: ' line to each text in the files given
# on the command line.  Project name is specified with 
# `-project' flag.

# If `-legacy' is given, add the `#atf: use legacy' 
# protocol as well.

my $legacy = 0;
my $project = '';
my $replace = 0;

GetOptions (
    'legacy+'=>\$legacy,
    'project:s'=>\$project,
    'replace'=>\$replace,
    );

die "atfproject.plx: must give -project <PROJECT> on command line\n"
    unless $project;

my @files = @ARGV;
if ($#files < 0) {
    @files = <00atf/*.atf>;
}

foreach my $f (@files) {
    open(F,"$f");
    my @lines = (<F>);
    close(F);
    warn "atfproject.plx: can't open `$f' for output\n" and next unless open(F,">$f");
    for (my $i = 0; $i <= $#lines; ++$i) {
	if ($lines[$i] =~ /^\x{feff}?\&/) {
	    my $proj = -1;
	    for (my $j = $i+1; $j <= $#lines; ++$j) {
		next if $lines[$j] =~ /^\s*$/;
		$proj = $j and last if $lines[$j] =~ /^\#project:/;
		last unless $lines[$j] =~ /^\#/;
	    }
	    print F $lines[$i];
	    if ($lines[$i+1] =~ /^\@composite/) {
		print F $lines[++$i];
	    }
	    if ($proj == -1) {
		print F "#project: $project\n";
		print F "#atf: use legacy\n" if $legacy;
	    } elsif ($replace) {
		$lines[$proj] = "#project: $project\n";
	    }
	} else {
	    print F $lines[$i];
	}
    }
    close(F);
}

1;
