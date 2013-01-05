#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
my $curr_file = '';
my @lines = ();

while (<>) {
    if (/SPACER/) {
	my($file,$line) = (/^(.*?):(.*?):/);
	if ($file ne $curr_file) {
	    if ($curr_file) {
		open(F,">$curr_file");
		print F @lines;
		close(F);
	    }
	    open(F,$file);
	    @lines = (<F>);
	    close(F);
	    $curr_file = $file;
	}
	$lines[$line-1] =~ s/^/\n\n\$ (SPACER)\n\n/;
    }
}

if ($curr_file) {
    open(F,">$curr_file");
    print F @lines;
    close(F);
}

1;
