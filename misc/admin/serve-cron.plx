#!/usr/bin/perl
use warnings; use strict;

sub xsystem;

my @BUILD_HOSTS = qw/oracc.museum.upenn.edu/;

foreach my $buildhost (@BUILD_HOSTS) {
    my @servelist = `curl -s http://$buildhost/srv`; chomp @servelist;
    foreach my $s (@servelist) {
	my($project,$datestamp) = split(/\s+/,$s);
	my $installstamp = ((stat("$ENV{'ORACC_BUILDS'}/$project/installstamp"))[9] || 0);
	if ($datestamp > $installstamp) {
	    xsystem "/bin/sh", "$ENV{'ORACC'}/bin/serve-project", $project, $buildhost;
	}
    }
}

sub
xsystem {
    warn "@_\n";
    system @_;
}

1;
