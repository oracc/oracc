#!/usr/bin/perl
use warnings; use strict;
use File::Find;

my @dirs = ();
my %dirs_seen = ();
my @files = ();
my $webdir = shift || '01bld/www';

find(\&webdirs, $webdir);

foreach my $d (@dirs) {
    my $target = $d;
    unless ($target =~ s,01bld/www,02www,) {
	$target = "02www/$target";
    }
    print "mkdir -p $target\n";
}

foreach my $f (@files) {
    my $target = $f;
    unless ($target =~ s,01bld/www,02www,) {
	$target = "02www/$target";
    }
    print "cp -af $f $target\n";
}

##########################################################################

sub
webdirs {
    push @dirs, $File::Find::dir unless $dirs_seen{$File::Find::dir}++;
    push @files, $File::Find::name if -f $_;
}

1;
