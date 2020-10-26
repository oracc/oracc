#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";

use ORACC::SMA::MorphData;
use Getopt::Long;
my $file = undef;
GetOptions(
    "f:s"=>\$file,
    );

use constant {
    RAW=>0,
    CBD=>1,
    FORMS=>2
};

my $mode = RAW;
$mode = FORMS if $file && $file =~ /\.forms/;
$mode = CBD if $file && $file =~ /\.glo/;

my @input = ();
if ($file) {
    open(F,$file) || die "$0: can't read from $file\n";
    @input = (<F>); chomp @input;
    close(F);
} else {
    $file = "<stdin>";
    while (<>) {
	push @input, $_;
    }
    chomp @input;
}

my @m = ();
if ($mode == RAW) {
    @m = map { $_ = form_morph($_) } @input;
} elsif ($mode == FORMS) {
    @m = map { $_ = form_morph($_) } @input;
} elsif ($mode == CBD) {
    @m = map { $_ = form_morph($_) } @input;
} else {
    die "$0: unknown mode value $mode\n";
}

my $line = 0;
foreach my $m (@m) {
    ++$line;
    next unless length $m;
    mdata_validate($m);
    foreach my $msg (mdata_messages()) {
	warn "$file:$line: $msg\n";
    }
}

########################################################################################

sub form_morph {
    my $x = shift;
    if ($x =~ /\@form(.*)$/) {
	my $t = $1; $t =~ s/^..*?\s\#//; $t =~ s/\s.*$//;
	return $t;
    } elsif ($x =~ /\s\#(\S+)/) {
	my $t = $1;
	return $t;
    } else {
	return '';
    }
}

1;
