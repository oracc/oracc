#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $cf = '';
my $pos = '';

while (<>) {
    if (/^\@entry/) {
	($cf,$pos) = (/\s(\S+)\s+\[.*?\]\s*(\S+)\s*$/);
    } elsif (/^\@parts/) {
	print;
	while (<>) {
	    last if /^\@end/;
	    print;
	}
    } elsif (/^\@sense/) {
	/\s+(\S+)/;
	my $epos = $1;
	if ($epos eq 'V') {
	    if ($pos =~ /^V/) {
		s/(\s+)(\S+)/$1$epos/;
	    } else {
		s#(\s+)(\S+)#$1$2/t#;
		warn "$.: forcing V/t\n";
	    }
	}
    } elsif (/^\@form/) {
	if (/\s\#([^\#]\S+)/) {
	    my $morph = $1;
	    if ($morph =~ s/\.~/:~/) {
		s/\#\S+/\#$morph/;
	    }
	    m/\$(.*?)/;
	    my $norm = $morph;
	    $norm =~ s/~/$cf/;
	    s/\$(\S+)/\$$norm/;
	}
    }
    print;
}

1;
