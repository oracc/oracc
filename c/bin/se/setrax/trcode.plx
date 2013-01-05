#!/usr/bin/perl
use warnings; use strict;
use integer;

my $mask = 0xfff0;
my $shift = 4;

my $coded = set_trcode(4,10);
get_trcode($coded);
get_branch($coded);

$coded = set_trcode(-6,10);
get_trcode($coded);
get_branch($coded);

sub
set_trcode {
    my ($in,$c) = @_;
    my $in2 = $in ^ ($c<<$shift);
    printf "trcode $c set on $in => %d\n",$in2;
    $in2;
}

sub
get_branch {
    my $c = shift;
    my $neg = 1;
    if ($c < 0) {
	$c *= -1;
	$neg = -1;
    }
    printf "brcode from $c is %d\n", ($c & ~$mask) * $neg;
}
sub
get_trcode {
    my $c = shift;
    $c *= -1 unless $c >= 0;
    printf "trcode from $c is %d\n", ($c & $mask)>>$shift;
}

1;
