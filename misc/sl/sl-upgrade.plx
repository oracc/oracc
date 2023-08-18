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

# revised useqs from the new useq generator
my %useq = ();
open(U,'upgrade-useq.log') || die;
while (<U>) {
    chomp;
    /generated useq (\S+) != (\S+)$/;
    $useq{$2} = $1;
}
close(U);

#load aka.tab to fix non-canonical compounds

my @aka = `grep -v '#' aka.tab`; chomp @aka;
my %aka = ();
foreach my $aka (@aka) {
    my($from,$to) = split(/\t/, $aka);
    $aka{$from} = $to;
}

my @subs = ('₀', '₁', '₂', '₃', '₄', '₅', '₆', '₇', '₈', '₉');
my $x = 1;

print '@signlist ogsl', "\n";

system 'cat', 'listdefs.txt';

while (<>) {
    chomp;
    next if /^\@signlist/;
    next if /^\@end\s+form/;

    s/\s+$//;

    s/OBZL/ABZL/g;
    
#    s/\@form\s+~\S+/\@form //;

    s/\@form\s+~[a-z]+/\@form/; # new convention, no ~tag with @form 
    
    if (/\@(form|sign)\s+(\S+)\s*/) {
	my ($sf,$nm) = ($1,$2);
	if ($aka{$nm}) {
	    print "\@$sf $aka{$nm}\n";
	    print "\@aka $nm\n";
	} else {
	    print "$_\n";
	}
    } else {

	s/\@v\?\s+(.*?)\s*$/\@v\t$1?/;
	s/\@nosign/\@sign-/;
	s/\@noform/\@form-/;
	s/\@nov/\@v-/;
	s#\@v\s+(/.*?/)#\@inote MC: $1/#;
	s#\@v-\s+/#\@inote MC:- /# && s#//#/#;
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
	s#\@v\s+(\d/\d)\s*$#\@inote \@v- $1#;
	s/\@fake\s*$/\@fake 1/;
	if (/\@v/ && /\.\.\./) {
	    my $orig = $_;
	    1 while s/\[\.\.\.\]/x/;
	    s/\.\.\./x/;
	    s/\s*$/ₓ/;
	    s/AŠ/aš/;
	    # warn "$orig => $_\n";
	}

	if (/^\@ucode\s+(.*?)\s*$/) {
	    my $u = $1;
	    if ($u =~ tr/././ == 0) {
		if ($u =~ /^x[eE]/) {
		    s/^/\@inote /;
		} else {
		    $u =~ s/^x//;
		    $_ = "\@list U+$u";
		}
	    } else {
		my ($useq) = (/\s(\S+)\s*$/);
		if ($useq{$useq}) {
		    $_ = "\@useq $useq{$useq}\n";
		} else {
		    s/ucode/useq/;
		}
	    }
	}
	s/uchar/utf8/;
	s/uphase/urev/;
	
	s/\?ₓ/ₓ?/;
	
	print "$_\n";
    }
}
print `cat comp.add`;

1;
