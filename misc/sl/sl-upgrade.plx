#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

# Update ogsl.asl to the new conventions used in the reimplementation

my @subs = ('₀', '₁', '₂', '₃', '₄', '₅', '₆', '₇', '₈', '₉');
my $x = 1;

print '@signlist ogsl', "\n";

while (<>) {
    chomp;
    next if /^\@signlist/;
    next if /^\@end\s+form/;
#    s/\@form\s+~\S+/\@form //;
    s/\@v\?\s+(.*?)\s*$/\@v\t$1?/;
    s/\@nosign/\@sign-/;
    s/\@noform/\@form-/;
    s/\@nov/\@v-/;
    s#\@v\s+/#\@inote MC: /#;
    s#\@v-\s+/#\@inote MC:- /#;
    if (/\@v\s+\?/) {
	my $tens = ($x / 10);
	my $units = ($x % 10);
	my $sub;
	$sub = $subs[$tens] if $tens > 0;
	$sub .= $subs[$units];
	s#\@v\s+\?#\@v\tx$sub#;
	++$x;
    }
    s/\@note:/\@note/;
    s/\@note\s*$/\@inote (empty note)/;
    s/\@v\s+\#old/\@inote #old/;
    s/\@v\s+\#nib/\@inote #nib/;
    s/\@v\s+\%/\@inote \%/;
    s#\@v\s+(\d/\d)#\@inote \@v- $1#;
    s/\@fake\s*$/\@fake 1/;
    print "$_\n";
}

1;
