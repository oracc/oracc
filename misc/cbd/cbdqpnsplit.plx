#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

my @g = ();
my @s = ();

my $split_lang = shift @ARGV;

die unless $split_lang;

while (<>) {
    if (/^\@form/) {
	if (/\%$split_lang/o) {
	    push @s, $_;
	} else {
	    push @g, $_;
	}
    } else {
	push @g, $_;
	push @s, $_;
    }
}

open(N,'>split-base.glo');
for (my $i = 0; $i < $#g; ++$i) {
    if ($g[$i] =~ /^\@entry/) {
	my $ok = has_form($i,@g);
	if ($ok) {
	    while ($g[$i]) {
		if ($g[$i] =~ /^\@bases/) {
		    print N $g[$i] unless $split_lang =~ /^sux/;
		} else {
		    print N $g[$i];
		}
		last if $g[$i] =~ /^\@end\s+entry/;
		last if $i > $#g;
		++$i;
	    }
	    print N "\n";
	}
    }
}
close(N);

open(S,'>split-lang.glo');
for (my $i = 0; $i < $#s; ++$i) {
    if ($s[$i] =~ /^\@entry/) {
	my $ok = has_form($i,@s);
	if ($ok) {
	    while ($s[$i]) {
		print S $s[$i];
		last if $s[$i] =~ /^\@end\s+entry/;
		last if $i > $#s;
		++$i;
	    }
	    print S "\n";
	}
    }
}
close(S);

sub has_form {
    my($i,@s) = @_;
    while ($i++ < $#s) {
	if ($s[$i] =~ /^\@form/) { return 1; }
	if ($s[$i] =~ /^\@end/) { return 0; }
    }
    return 0;
}

1;

