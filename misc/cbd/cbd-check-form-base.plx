#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; 
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::SL::BaseC;
use ORACC::SMA::StripNSF;

my $autofix = 0;
my $ignore = '';
use Getopt::Long;
GetOptions(
    'a'=>\$autofix,
    'i:s'=>\$ignore,
    );

my %ignore = ();
if ($ignore) {
    open(I, $ignore) || die "$0: can't open ignore list $ignore\n";
    while (<I>) {
	chomp;
	s/\s+/\t/g;
	s/^\s*//;
	s/\s*$//;
	++$ignore{$_};
    }
    close(I);
}

ORACC::SL::BaseC::init();
my $file = shift @ARGV;
my $line = 0;
my $pos = 'N';
die "$0: must give file on command line\n" unless $file;
open(F,$file) || die "$0: can't open file $file\n";
while (<F>) {
    ++$line;
    if (m-^\@form\S*\s+(\S+).*?/(\S+).*?#(\S+)-) {
	my($form,$base,$morph) = ($1,$2,$3);

	next if $ignore{"$form\t$base"};

	my $form_no_bs = $form;
	$form_no_bs =~ s/\\.*$//;
	my $fs = ORACC::SL::BaseC::tlit_sig('',$form_no_bs);
	my $bs = ORACC::SL::BaseC::tlit_sig('',$base);
	
	if ($pos =~ /N$/ && $fs !~ /^$bs/) {
	    if ($fs =~ /$bs$/ || (($fs=~tr/././)==($bs=~tr/././)) && !$morph) {
		if ($autofix) {
		    s#/(\S+)#/$form#;
		} else {
		    warn "$file:$line: noun $form should have base $form (instead of $base)\n";
		}
	    } else {
		if ($fs =~ /q/ || $bs =~ /q/) {
		    warn "$file:$line: noun $form doesn't begin with $base ($fs != $bs)\n";
		} else {
		    warn "$file:$line: noun $form doesn't begin with $base\n";
		}
	    }
	    my $snsf = strip_nsf($form_no_bs);
	    my $b = $form_no_bs;
	    my $m = '';
	    if ($snsf) {
		$b = $snsf;
		$m = $form_no_bs;
		$m =~ s/^$b//;
	    }
	    warn "$file:$line: for form $form use base $b morph $m\n";
	} elsif (!$morph && length($bs) != length($fs)) {
	    warn "$file:$line: $form /$base may need morph\n";
	}
    }
    print if $autofix;
}
close(F);

1;
