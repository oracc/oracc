#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my %missing = ();
open(M,'not-in-ogsl.lst') || die;
while (<M>) {
    chomp;
    ++$missing{$_};
}
close(M);

# warn Dumper \%missing;

my %m = ();

open(N,'not-in-ogsl-inst.tab');
while (<N>) {
    chomp;
    my(@f) = split(/\t/,$_);
    my $f = $f[0]; $f =~ s/^.*?://; # remove .tab file info that grep put in
    my $n = $f[1];
    my @g = split(/\s+/,$f[$#f]);
    my @ng = find_missing(@g);
    if ($#ng < 0) {
	warn "no missing graphemes found in @g\n";
    }
    push @{$m{"$f:$n"}}, @ng;
}
close(N);

open(A,'>not-in-ogsl.atf') || die;
print A '&X990990 = Not in OGSL', "\n#project: epsd2\n#atf: use unicode\n#atf: use math\n";
foreach my $l (sort keys %m) {
    print A "$l. @{$m{$l}}\n";
}
close(A);

#############################################################################

sub find_missing {
    my @g = @_;
    my @m = ();
    foreach my $g (@g) {
	push @m, $g if $missing{$g};
    }
    @m;
}

1;
