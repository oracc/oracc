#!/usr/bin/perl -C0
use warnings; use strict;
my %chars = ();
while (<>) {
    my($file,$char,$font) = (/^(.*?): char `(.*?)'.*font (\S+)$/);
    if ($file) {
	push @{$chars{"$font\: $char: "}}, $file;
    }
}
foreach my $fc (sort keys %chars) {
    print $fc, (sort @{$chars{$fc}})[0], "\n";
}

1;
