#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use Getopt::Long;

my $ext = 'atf';
my $printable = 0;
my $tab_file = 'filenames.tab';
my %tab = ();

GetOptions(
    'ext:s'=>\$ext,
    );

die("rename-files.plx: must run from directory with new/ atf/ and filenames.tab\n")
    unless -d 'new' && -d 'atf' && -r $tab_file;

open(T,$tab_file);
while (<T>) {
    my($p,$n) = (/^(.*?)\t(.*?)\s*$/);
    check_filename($n);
    $tab{$p} = $n;
}
close(T);

my @files = <atf/*>;
foreach my $f (@files) {
    next if /~$/; # ignore Emacs backup files
    open(F,$f) || die "rename-files.plx: unable to read $f\n";
    while (<F>) {
	if (/^.?\&([PQX]\d+)/) {
	    my $p = $1;
	    if ($tab{$p}) {
		open(N,">new/$tab{$p}.$ext");
		print N;
		$printable = 1;
		delete $tab{$p};
	    } else {
		warn "rename-files.plx: $p has no entry in filenames.tab\n";
	    }
	} else {
	    if ($printable) {
		print N;
	    } else {
		warn "rename-files.plx: $f: junk before first \&-line\n";
	    }
	}
    }
}

if (scalar keys %tab) {
    warn <<EOW;
rename-files.plx: the following IDs from filenames.tab were not found in the
input files atf/*:
EOW
    warn join("\n",sort keys %tab), "\n";
}

###################################################################################

sub
badfile {
    my $x = shift;
    warn("rename-files.plx: $x: filename ", @_, "\n");
}

sub
check_filename {
    my $x = shift;
    badfile("too long (maximum is 16 characters)") if length($x) > 16;
    badfile("must not end with underscore\n") if $x =~ /_$/;
    $x =~ tr/a-zA-Z0-9_//d;
    badfile("has illegal characters `$x'") if length $x;
}

1;
