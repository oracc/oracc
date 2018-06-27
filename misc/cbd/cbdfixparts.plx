#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $curr_file = undef;
my $file = shift @ARGV;
open(F,$file) || die;
my @f = (<F>);
close(F);

while (<>) {
    my($file,$line,$from,$to) = (/^(.*?):(\d+):\s+part\s+(.*?)\s+better\s+(.*?)\s*$/);
    --$line;
    if ($file) {
	if ($f[$line] =~ /^\@parts/) {
	    warn "$file:$line: subbing $from for $to\n";
	    $f[$line] =~ s/\Q$from/$to/;
	} else {
	    warn "$file:$line: not a \@parts line\n";
	}
    }
}

print @f;

1;
