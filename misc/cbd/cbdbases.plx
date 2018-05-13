#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Util;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Bases;

use Getopt::Long;

%ORACC::CBD::bases = ();
%ORACC::CBD::forms = ();

# bare: no need for a header
# check: only do validation
# kompounds: check compounds (like |SI.A|) against OGSL
# dry: no output files
# edit: edit cbd via acd marks and write patch script
# filter: read from STDIN, write CBD result to STDOUT
# force: generate output CBD even if there are errors
# reset: reset cached glo and edit anyway 
# trace: print trace messages 
# vfields: only validate named fields, plus some essential ones supplied automatically

my $project = '';
my $lang = '';

my %args = ();
GetOptions(
    \%args,
    qw/cbd:s log:s out:s/,
    ) || die "unknown arg";

foreach my $f (qw/cbd log out/) {
    if ($args{$f}) {
	if ($f eq 'cbd' || $f eq 'log') {
	    die "cbdbases.plx: can't read $f file $args{$f}\n"
		unless -r $args{$f};
	} else {
	    die "cbdbases.plx: can't write $f file $args{$f}\n"
		unless open(O,">$args{'out'}");
	}
    } else {
	die "cbdbases.plx must give -$f <filename> on command line\n";
    }
}

pp_file($args{'cbd'});
my @cbd = pp_load(\%args);
do_bases(\%args, @cbd);

sub do_bases {
    my($args,@cbd) = @_;
    ($project,$lang) = @$args{qw/project lang/}	;
    my $is_compound = 0;
    for (my $i = 0; $i <= $#cbd; ++$i) {
	next if $cbd[$i] =~ /^\000$/ || $cbd[$i] =~ /^\#/;
	pp_line($i+1);
	if ($cbd[$i] =~ /^\@entry\s+(.*?)\[/) {
	    my $cf = $1;
	    $is_compound = ($cf =~ tr/ / /);
	} elsif ($cbd[$i] =~ /^\@bases/) {
	    my @log_errors = bases_log_errors($i);
	    my %b = bases_hash($cbd[$i], $is_compound);
	    bases_fix(\%b,@log_errors);
	}
    }
}

1;
