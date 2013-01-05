#!/usr/bin/perl
use warnings; use strict; use open ':utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::SL::Base;
use Getopt::Long;

my $how = 'ids'; # 'tra'=transliteration, 'cun'=cuneiform, 'htm'=html
my $what = 'sign';

my @what = qw/values sign homophones list compounds contains contained cinit clast/;
my %what; @what{@what} = ();

GetOptions(
    'how:s'=>\$how,
    'what:s'=>\$what,
);

my @args = @ARGV;

ORACC::SL::Base::init();
if ($#args >= 0) {
    foreach my $arg (@args) {
	my @res = (lookup($arg));
	if ($#res >= 0) {
	    display(@res);
	} else {
	    display("#no $what match for $arg");
	}
    }
} else {
    while (<>) {
	chomp;
	my @res = (lookup($_));
	if ($#res >= 0) {
	    display(@res);
	} else {
	    display("#no $what match for $_");
	}
    }
}
ORACC::SL::Base::term();

##############################################################

sub
display {
    my @disp = @_;
    print join("\n", @disp), "\n";
}

sub
lookup {
    my $x = shift;
    my @r = ();
    if ($what eq 'sign') {

	my $r = ORACC::SL::Base::xid($x);
	@r = ($r) if $r;

    } elsif ($what eq 'homophones') {

	@r = ORACC::SL::Base::homophones($x);

    } elsif ($what eq 'values') {

	@r = ORACC::SL::Base::values($x);

    } elsif ($what eq 'list') {

	@r = ORACC::SL::Base::list($x);

    } elsif ($what eq 'compounds') {
	
    } elsif ($what eq 'contains') {
	
    } elsif ($what eq 'contained') {
	
    } elsif ($what eq 'cinit') {
	
    } elsif ($what eq 'clast') {
	
    } else {
	die "sl-se.plx: bad 'what' argument: `$what' unknown\n";
    }
    @r;
}

1;
