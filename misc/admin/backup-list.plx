#!/usr/bin/perl
use warnings; use strict;
use File::Find;
#use File::chdir;

my $deep = 0;
if ($ARGV[0] eq '-deep') {
    $deep = 1;
    shift @ARGV;
}
my $reffile = shift @ARGV;
my $refdate = ($reffile ? (stat($reffile))[9] : 0);

#warn "$refdate\n";

dotfiles();

my $cdir = '';
my @dirs = grep !/00any/, <00*>;
find(\&wanted, @dirs);

if ($deep) {
    my @subs = `list-all-subs.sh`; chomp @subs;
    foreach my $s (@subs) {
#	local $CWD = $s;
#	$cdir = "$s/";
	@dirs = grep !/00any/, eval "<$s/00*>";
	find(\&wanted, @dirs);
    }
}

########################################################################

sub dotfiles {
    print ".bashrc\n" if -r '.bashrc';
    print ".bash_profile\n" if -r '.bash_profile';
    print ".emacs\n" if -r '.emacs';
    print ".profile\n" if -r '.profile';
}

sub
wanted {
    if (-f $_) {
	if (/~$/
	    || $File::Find::name =~ m#00web/esp/(?:http|back|temp)#) {
	} else {
	    if ($refdate) {
		my $fdate = (stat($_))[9];
		print "$cdir$File::Find::name\n"
		    if $fdate > $refdate;
	    } else {
		print "$cdir$File::Find::name\n";
	    }
	}
    }
}
1;
