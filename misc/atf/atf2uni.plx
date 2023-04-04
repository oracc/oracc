#!/usr/bin/perl
#
# Trivial wrapper for the ORACC::ATF::Unicode.pm module, q.v.
#
# Steve Tinney 04/03/01, for PSD and ORACCI projects.
#
# v1.0.  Placed in the Public Domain.
use warnings; use strict; use open ':utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Getopt::Long;
use lib "$ENV{'ORACC'}/lib";
use ORACC::ATF::Unicode;

my $lem_only = 0;
my $uni_proto = 0;
GetOptions(
    lem=>\$lem_only,
    uni=>\$uni_proto,
    );

while (<>) {
    if ((/^(\S+\.\s)(.*)$/ || /^(==\S+\s+)(.*)$/) && !$lem_only) {
	my($lnum,$text) = ($1,$2);
	chomp;
	my $u = ORACC::ATF::Unicode::gconv($text);
#	warn "$u\n";
#	1 while $u =~ s/([^a-zA-ZšŠṣṢṭṬŋŊḫḪ₀-₉])([₀-₉]+\()/updig($1,$2)/e;
#	1 while $u =~ s/(^|[-\s])([₀-₉]+\()/updig($1,$2)/e;
#	warn "$u\n";
	1 while $u =~ s#([^a-zŋṣšṭA-ZŊṢŠṬ][₁₂₃₄₅₆₇₈₉₀]+(?:/[0-9]|\(|\]|⸣|[\#*?!]|>))#updig('',$1)#e;
	1 while $u =~ s#([[⸢][₁₂₃₄₅₆₇₈₉₀]+)#updig('',$1)#e;
	1 while $u =~ s#(LAK|REŠ)([₀-₉])#updig($1,$2)#e;
	1 while $u =~ s/REŠ([0-9])/REC$1/;
	1 while $u =~ s/([a-zŋšṣṭ])×\(/$1ₓ(/;
	print "$lnum$u", "\n";
    } elsif (s/^(\#lem:\s+)//) {
	print $1;
	chomp;
	my @bits = grep(defined, split(/;\s+/, $_));
	for (my $i = 0; $i <= $#bits; ++$i) {
	    if ($bits[$i] =~ /\[/) {
		my($cf,$rest) = ($bits[$i] =~ /^(.*?)(\[.*)$/);
		print ORACC::ATF::Unicode::gconv($cf), $rest;
	    } else {
		print $bits[$i];
	    }
	    print '; ' if $i < $#bits;
	}
	print "\n";
    } else {
	print;
	if (/^#project/) {
	    print "#atf: use unicode\n" if $uni_proto;
	}
    }
}

sub
updig {
    my ($az,$x) = @_;
    $az = '' unless $az;
    $x =~ tr/₀-₉/0-9/;
    "$az$x";
}

1;

__END__

=head1 NAME

atf2uni.plx -- convert ATF character notations to Unicode

=head1 SYNOPSIS

atf2uni.plx [file]

=head1 DESCRIPTION

atf2uni.plx is a simple filter which translates the ATF character notations to Unicode.

=head1 AUTHOR

Steve Tinney (stinney@sas.upenn.edu)

=head1 COPYRIGHT

Copyright(c) Steve Tinney 2004.

Released under the Gnu General Public License
(http://www.gnu.org/copyleft/gpl.html).
