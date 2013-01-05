#!/usr/bin/perl
use warnings; use strict;
use Getopt::Long qw(:config no_ignore_case);

my $cgimode = 0;
my $from;
my $genre;
my $owner;
my $period;
my $prefix;
my $roman = 0;
my $Roman = 0;
my $to;

if ($#ARGV >= 0) {
    GetOptions(
	'genre:s'=>\$genre,
	'owner:s'=>\$owner,
	'period:s'=>\$period,
	'prefix:s'=>\$prefix,
	'roman+'=>\$roman,
	'Roman+'=>\$Roman,
	'from:s'=>\$from,
	'to:s'=>\$to,
	);
}

if (!$prefix) {
    use CGI;
    my $cgi = new CGI;
    $cgimode = $cgi->param('cgimode');
    $genre = $cgi->param('genre');
    $owner = $cgi->param('owner');
    $period = $cgi->param('period');
    $prefix = $cgi->param('prefix');
    $from   = $cgi->param('from');
    $to     = $cgi->param('to');
    if ($cgi->param('nums')) {
	if ($cgi->param('nums') eq 'roman') {
	    $roman = 1;
	} elsif ($cgi->param('nums') eq 'Roman') {
	    $roman = $Roman = 1;
	}
    }
    print $cgi->header(-type=>'text');
}

if ($cgimode) {
    cusage(1) unless $prefix && $from && $to;
} else {
    usage(1) unless $prefix && $from && $to;
    usage(2) unless $from =~ /^\d+$/ && $to =~ /^\d+$/;
}

my $xfields = '';
foreach my $f ($genre,$owner,$period) {
    if ($f) {
	$xfields .= "$f\t";
    }
}

$roman = 1 if $Roman;

if ($roman) {
    Roman::init();
}

foreach my $i ($from..$to) {
    if ($roman) {
	print "$prefix pl. ", $Roman ? Roman::Roman($i) : Roman::roman($i),"\t$xfields\n";
    } else {
	print "$prefix, $i\t$xfields\n";
    }
}

sub
cusage {
    my $e = shift;
    print <<EOC;
voltemplate.plx generates placeholder entries for text volumes.

On the webservice, both from and to boxes must contain digits; prefix
must be a non-empty string.
EOC
}

sub
usage {
    my $e = shift;
    warn("voltemplate.plx: generate placeholder entries for text volumes\n\n");
    warn("Usage\n    voltemplate.plx [-r|-R] -prefix [string] -from [num] -to [num]\n\n");
    if ($e == 2) {
	warn("Values for -from and -to must be all digits\n\n");
    }
    warn("  * This program writes to stdout; redirect output using >.\n\n");
    warn("  * Every line has a <TAB> at the end to make it easy to add more fields.\n\n");
    warn("  * To include spaces in -prefix, use quotes around the prefix.\n\n");
    warn("  * Hint: the options can be abbreviated to -p, -f and -t\n\n");
    warn("  * For roman numerals use the -r (lowercase) or -R (uppercase) options\n\n");

    warn("Example:\n\n");
    warn("    voltemplate.plx -prefix 'UET 6/1' -from 1 -to 300 >uet6.lst\n");
    exit(1);
}

### Adapted/cut down from
###   http://search.cpan.org/src/OZAWA/Roman-1.1/Roman.pm
### by Steve Tinney 2006-10-30.

package Roman;

my %roman_digit = ();
my @figure = ();

=head1 NAME

Roman - Perl module for conversion between Roman and Arabic numerals.

=head1 SYNOPSIS

	use Roman;

	$roman = Roman($arabic);
	$roman = roman($arabic);

=head1 DESCRIPTION

This package provides some functions which help conversion of numeric
notation between Roman and Arabic.

=head1 BUGS

Domain of valid Roman numerals is limited to less than 4000, since
proper Roman digits for the rest are not available in ASCII.

=head1 CHANGES

1997/09/03 Author's address is now <ozawa@aisoft.co.jp>

=head1 AUTHOR

OZAWA Sakuro <ozawa@aisoft.co.jp>

=head1 COPYRIGHT

Copyright (c) 1995 OZAWA Sakuro.  All rights reserved.  This program
is free software; you can redistribute it and/or modify it under the
same terms as Perl itself.

=cut

sub init {
    %roman_digit = qw(1 IV 10 XL 100 CD 1000 MMMMMM);
    @figure = reverse sort keys %roman_digit;
    grep($roman_digit{$_} = [split(//, $roman_digit{$_}, 2)], @figure);
}

sub Roman {
    my($arg) = shift;
    0 < $arg and $arg < 4000 or return undef;
    my($x, $roman);
    foreach (@figure) {
        my($digit, $i, $v) = (int($arg / $_), @{$roman_digit{$_}});
        if (1 <= $digit and $digit <= 3) {
            $roman .= $i x $digit;
        } elsif ($digit == 4) {
            $roman .= "$i$v";
        } elsif ($digit == 5) {
            $roman .= $v;
        } elsif (6 <= $digit and $digit <= 8) {
            $roman .= $v . $i x ($digit - 5);
        } elsif ($digit == 9) {
            $roman .= "$i$x";
        }
        $arg -= $digit * $_;
        $x = $i;
    }
    $roman;
}

sub roman {
    lc Roman shift;
}

1;
